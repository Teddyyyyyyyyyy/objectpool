#ifndef OBJECTPOOL2_HPP
#define OBJECTPOOL2_HPP
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
    struct PoolObject
    {
        TObject *object;
        Timestamp last_used;
        Timestamp borrowed_at;
    };
private:
    size_t maxTotal;  // 池可以创建对象的最大个数
    size_t maxIdle;   // 空闲队列中的最大空闲对象个数
    size_t minIdle;   // 空闲队列中的最小空闲对象个数 
    size_t maxIdleTime;  //最大空闲时间 s
    
    size_t totalObject = 0;
    std::list<PoolObject> pool;
    
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
        evecTimer.init(maxIdleTime*1000);
        evecTimer.add_timer(timer);
    }
    // 获取对象
    template <class... Args>
    std::shared_ptr<TObject> acquire(Args &&...args)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!cv_.wait_for(lock,std::chrono::milliseconds(1000),
            [&](){ return !pool.empty() || totalObject < maxTotal;}))
        {
            return std::shared_ptr<TObject>(nullptr);
        }
        if (pool.empty())
        { 
            ++totalObject;
            return std::shared_ptr<TObject>(new TObject(std::forward<Args>(args)...),
            [&](TObject *p) { release(p); });
        }
        else 
        {
            auto ptr = pool.front().object;
            pool.pop_front();
            return std::shared_ptr<TObject>(ptr,[&](TObject *p){ release(p); });
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
            delete pObj.object;
        }
        pool.clear();
    }
    ~ObjectPool()
    {
        clear();
    }

private:

    void release(TObject *ptr) // 释放
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (pool.size() < maxIdle)
        {
            pool.push_back({ptr,Timestamp::Now()});
        }
        else
        {
            cout<<"对象池满了...释放对象 "<<endl;
            --totalObject;
            delete ptr;
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
            auto idleTime = Timestamp::Now() - it->last_used;
            if(idleTime >= maxIdleTime)
            {
            cout<<"超时 ... 驱逐对象..."<<endl;
            --totalObject;
            delete it->object;
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