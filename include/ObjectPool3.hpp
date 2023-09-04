#ifndef OBJECTPOOL3_HPP
#define OBJECTPOOL3_HPP
#include"Timestamp.hpp"
#include"Timer.hpp"
#include"EvectionTimer.hpp"

#include<list>
#include<mutex>
#include<condition_variable>
#include<thread>
using namespace std;



template <typename TObject>
class ObjectPool
{
private:
    struct PoolObject : public TObject
    {
        template<class...Args>
        PoolObject(Args &&... args)
            :TObject(std::forward<Args>(args)...),
            borrowed_at(Timestamp::Now())
        {}
        PoolObject(const PoolObject &obj)
            :TObject(obj),last_used(Timestamp::Now())
        {}
        Timestamp last_used;
        Timestamp borrowed_at;
        int tag;//1 idle;2 act; 3 exile;fangzhu
    };
private:
    size_t maxTotal;  // 池可以创建对象的最大个数
    size_t maxIdle;   // 空闲队列中的最大空闲对象个数
    size_t minIdle;   // 空闲队列中的最小空闲对象个数 
    size_t maxIdleTime;  //最大空闲时间 s
    size_t maxborrTime = 10;//s
    size_t totalObject = 0;
    std::list<PoolObject *> pool;
    std::list<PoolObject *> actpool;

    std::mutex mutex_;
    std::condition_variable cv_;
    
    EvectionTimer evecTimer;
public:
    ObjectPool(size_t maxtotal = 16, size_t maxidle = 8, size_t minidle = 2,
               size_t maxidletime =60)
        : maxTotal(maxtotal),
    	maxIdle(maxidle),
    	minIdle(minidle),
    	maxIdleTime(maxidletime)
    {
        Timer timer;
        timer.init();
        timer.set_timer(std::bind(&ObjectPool::evictionLoop,this),maxIdleTime*1000);

        Timer ext;
        ext.init();
        ext.set_timer(std::bind(&ObjectPool::deportloop,this),3000);
        evecTimer.init(maxIdleTime*1000);
        evecTimer.add_timer(timer);
        evecTimer.add_timer(ext);
    }
    // 获取对象
    template <class... Args>
    std::shared_ptr<PoolObject> acquire(Args &&...args)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!cv_.wait_for(lock,std::chrono::milliseconds(1000),
            [&](){ return !pool.empty() || totalObject < maxTotal;}))
        {
            return std::shared_ptr<PoolObject>(nullptr);
        }
        if (pool.empty())
        { 
            ++totalObject;
            auto ptr = std::shared_ptr<PoolObject>(new PoolObject(std::forward<Args>(args)...),
            [&](PoolObject *p) { release(p); });
            ptr->tag = 2;
            actpool.push_back(ptr.get());
            return ptr;
        }
        else 
        {
            auto ptr = pool.front();
            pool.pop_front();
            ptr->borrowed_at = Timestamp::Now();
            ptr->tag = 2;
            actpool.push_back(ptr);
            return std::shared_ptr<PoolObject>(ptr,[&](PoolObject *p){ release(p); });
        }
    }
    
    // 池大小
    size_t getIdleObjSize() const
    {
        return pool.size();
    }
    size_t getTotalObjSize() const
    {
        return totalObject;
    }
    size_t getActivateObjSize() const
    {
        return totalObject - pool.size();
    }
    
    void clear()
    {
        evecTimer.set_stop();
        for (auto &pObj : pool)
        {
            delete pObj;
            pObj = nullptr;

        }
        pool.clear();
    }
    ~ObjectPool()
    {
        clear();
    }

private:

    void release(PoolObject *ptr) // 释放
    {
        std::unique_lock<std::mutex> lock(mutex_);
        actpool.remove(ptr);
        if(ptr->tag == 3)
        {
            delete ptr;
            cout<<"Tobject 放逐"<<endl;
        }
        if (pool.size() < maxIdle )
        {
            cout <<"pool.push_back"<<endl;
            ptr->last_used = Timestamp::Now();
            ptr->tag = 1;
            pool.push_back(ptr);
        }
        else
        {
            cout<<"对象池满了...释放对象 or BorrTime"<<endl;
            --totalObject;
            delete ptr;
        }
        cv_.notify_one();
    }
    void deportloop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if(actpool.size() <= 0) 
        {
            cout<<"acepool.empty"<<endl;
            return;
        }
        auto it = actpool.begin();
        for(;it != actpool.end();++it)
        {
            auto actTime = Timestamp::Now() - (*it) ->borrowed_at;
            if(actTime >= maxborrTime)
            {
                (*it)->tag = 3;
                --totalObject;
                cout<<"标记对象为放逐"<<endl;
            }
        }
        cv_.notify_one();
    }
    void evictionLoop()
    {
        //while(!stopEvictionThread)
        std::unique_lock<std::mutex> lock(mutex_);
        if(pool.size() <= minIdle) return; 
        int num = pool.size() - minIdle;
        num = num > 3 ?  3 : num;
        auto it = pool.begin();
        for(; it != pool.end(); )
        {
            auto idleTime = Timestamp::Now() - (*it)->last_used;
            if(idleTime >= maxIdleTime)
            {
            cout<<"超时 ... 驱逐对象..."<<endl;
            --totalObject;
            delete (*it);
            pool.erase(it++);
            if(--num == 0) break;
            }
            else
            {
            ++it;
            }
        }
    }


};

#endif 