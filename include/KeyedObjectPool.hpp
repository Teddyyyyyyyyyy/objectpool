#ifndef KeyedObjectPool_HPP
#define KeyedObjectPool_HPP
#include"Timestamp.hpp"
#include"Timer.hpp"
#include"EvectionTimer.hpp"

#include<list>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<map>

using namespace std;



template <typename TKey, typename TObject>
class KeyedObjectPool
{
private:
    struct PoolObject : public TObject
    {
        friend class KeyedObjectPool;
        template<class... Args>
        PoolObject(Args &&... args)
            :TObject(std::forward<Args>(args)...),
            borrowed_at(Timestamp::Now()) 
        {}
        PoolObject(const PoolObject &obj)
            :TObject(obj),last_used(Timestamp::Now())
        {

        }
    private:
        Timestamp last_used;
        Timestamp borrowed_at;
        int tag; // 1 idle ; 2 act;  3 exile;
    };
private:
    size_t maxTotal;  // 池可以创建对象的最大个数
    size_t maxIdle;   // 空闲队列中的最大空闲对象个数
    size_t minIdle;   // 空闲队列中的最小空闲对象个数 
    size_t maxIdleTime;  //最大空闲时间 60s
    size_t maxborrTime = 10;  //s
    
    size_t totalObject = 0;
    //std::list<PoolObject *> pool; // idlepool;
    std::map<TKey,std::list<PoolObject*> > pool;
    //       key  _  value 
    // std::multimap<TKey,PoolObject * > pool;
    std::list<PoolObject *> actpool;
    
    std::mutex mutex_;
    std::condition_variable cv_;

    EvectionTimer evecTimer;
public:
    //6,4,2,5 
    KeyedObjectPool(size_t maxtotal = 16, size_t maxidle = 8,
               size_t minidle = 2,size_t maxidletime =60)
        : maxTotal(maxtotal),
    	maxIdle(maxidle),
    	minIdle(minidle),
    	maxIdleTime(maxidletime)
    {
        Timer timer;
        timer.init();
        timer.set_timer(std::bind(&KeyedObjectPool::evictionLoop,this),maxIdleTime*1000);
       
        Timer ext;
        ext.init();
        ext.set_timer(std::bind(&KeyedObjectPool::deportLoop,this),3000);

        evecTimer.init(maxIdleTime *1000);
        evecTimer.add_timer(timer);
        evecTimer.add_timer(ext);

       // clog<<"KeyedObjectPool ... "<<endl;
    }

    //std::map<TKey,std::list<PoolObject*> > pool;
    // 获取对象
    template <class... Args>
    std::shared_ptr<PoolObject> acquire(const TKey &key,Args &&...args)
    {
        //clog<<"acquire..."<<endl;
        std::unique_lock<std::mutex> lock(mutex_);
        if(!cv_.wait_for(lock,std::chrono::milliseconds(1000),
            [&](){ return !pool[key].empty() || totalObject < maxTotal;}))
        {
            return std::shared_ptr<PoolObject>(nullptr);
        }
        if (pool[key].empty())
        { 
        
            ++totalObject;
            //cout<<"acquire ..."<<totalObject<<endl;
            auto ptr =  std::shared_ptr<PoolObject>(new PoolObject(std::forward<Args>(args)...),
                                      [&](PoolObject *p) { release(key,p); });
            ptr->tag = 2;
            actpool.push_back(ptr.get());  
            return ptr;              
        }
        else 
        {
            auto ptr = pool[key].front();
            pool[key].pop_front();
            ptr->borrowed_at = Timestamp::Now();
            ptr->tag = 2;
            actpool.push_back(ptr);
            return std::shared_ptr<PoolObject>(ptr,[&](PoolObject *p){ release(key,p); });
        }
    }
    
    // 池大小
    size_t getIdleObjSize() const
    {
        return getTotalIdleObjNum();
    }
    size_t getTotalObjSize() const
    {
        return totalObject;
    }
    size_t getActivateObjSize() const
    {
        return totalObject - getIdleObjSize();
    }
    
    void clear()
    {
        evecTimer.set_stop();
        for (auto &pObj : pool)
        {
            for(auto &obj : pObj.second)
            {
                delete obj;
                obj = nullptr;
            }
            pObj.second.clear();
        }
        pool.clear();
    }
    ~KeyedObjectPool()
    {
        clear();
    }

private:
    size_t getTotalIdleObjNum() const
    {
        size_t sum = 0;
        for(auto &x : pool)
        {
            sum = sum + x.second.size();
        }
        //cout<<"sum : "<<sum<<endl;
        return sum;
    }

//std::map<TKey,std::list<PoolObject*> > pool;
    void release(const TKey &key, PoolObject *ptr) // 释放
    {
        std::unique_lock<std::mutex> lock(mutex_);
        actpool.remove(ptr);
        if(ptr->tag == 3)
        {
            delete ptr;
            //cout<<"Tobject 放逐"<<endl;
            return ;
        }
        //if (pool[key].size() < maxIdle)
        if(getTotalIdleObjNum() < maxIdle)
        {
           // cout<<"pool.push_back"<<endl;
            ptr->last_used = Timestamp::Now();
            ptr->tag = 1;
            pool[key].push_back(ptr);
        }
        else
        {
            //cout<<"对象池满了...释放对象  "<<endl;
            --totalObject;
            //cout<<"release: "<<totalObject<<endl;
            delete ptr;
        }
        cv_.notify_one();
    }
    void deportLoop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if(actpool.size() <= 0) 
        {
            //cout<<"actpool.empty"<<endl;
            return ;
        }
        auto it = actpool.begin();
        for(;it != actpool.end(); ++it)
        {
            auto actTime = Timestamp::Now() - (*it)->borrowed_at;
            if((*it)->tag != 3 && actTime >= maxborrTime)
            {
                (*it)->tag = 3;
                --totalObject;
                //cout<<"deportobj: "<<totalObject<<endl;
                //cout<<" 标记对象为放逐"<<endl;
            }
        }
        cv_.notify_one();
    }
    //std::map<TKey,std::list<PoolObject*> > pool;
    void evictionLoop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(getTotalIdleObjNum() <= minIdle) return ;
        int num = getTotalIdleObjNum()-minIdle;
        num = num > 3 ?  3 : num;
        auto it = pool.begin(); //map=>pair<Key,std::list<PoolObject*>> 
        //std::cout<<"pool.size(): "<<pool.size()<<std::endl;
        for(; it != pool.end(); ++it)
        {
            std::cout<<"for(; it != pool.end():)"<<std::endl;
            auto objit = it->second.begin();
            for(; objit != it->second.end(); ++objit)
            {
                auto idleTime = Timestamp::Now()- (*objit)->last_used;
                if(idleTime >= maxIdleTime)
                {
                    //cout<<"超时 ... 驱逐对象..."<<endl;
                    --totalObject;
                    //cout<<"evictionLoop: "<<totalObject<<endl;
                    delete *objit;
                    it->second.erase(objit);
                    --num;
                    break;  // 
                }
            }
            if(num == 0) break;
        }
    }
};

#endif 