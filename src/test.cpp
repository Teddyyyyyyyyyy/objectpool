#include"../include/CDConnPool.hpp"
#include"../include/Timestamp.hpp"
#include<thread>
using namespace std;
const int len = 128;
const int n = 5000;

void funa(CDConnPool &pool)
{
    char buff[len]={};
    for(int i =0;i<n;++i)
    {
        sprintf(buff,"insert into student(sid,sname,ssex,sage) values('A0A00%d', 'yhping', '%s' ,%d)",i+1,(rand()%2 ? "man":"woman"),rand()%10+15);
        pool.ExecuteSQL(buff);
    }
}

int main()
{
    CDConnPool connpool("localhost","root","123456","c2205db",3306);
    Timestamp begin;
    begin.now();
    std::thread tha(funa,std::ref(connpool));
    tha.join();
    auto sec = Timestamp::Now() - begin;
    cout<<"sec : "<<sec<<endl;
    return 0;
}





#if 0
#include"../include/MySqlApi3.hpp"

#include<string>
#include<iostream>
#include<vector>
using namespace std;

int main()
{
    CDataConn dbConn;
    const int LEN = 256;
    char buff[LEN]={};
    if(!dbConn.InitConn("localhost","root","hm123456","GYStudent",3306))
    {
        clog<<" conn faild "<<endl;
        return 1;
    }
    std::vector<std::vector<std::string> > dbset,sset;
    sprintf(buff,"select * from student");
    int ret = 0;
    if(dbConn.GetRecordSet(buff) == 0)
    {
        ret = dbConn.GetRecordSet(dbset);
        //dbConn.FreeResult();
    }
    cout<<"ret: "<<ret<<endl;
    int nrow = dbset.size();
    for(int i = 0;i<nrow; ++i)
    {
        int feildnum = dbset[i].size();
        for(int j = 0;j<feildnum; ++j)
        {
            cout<<dbset[i][j]<<"\t";
        }
        printf("\n------------------------------\n");
    }
    cout<<endl;
    dbConn.GetRecordSet(buff);
    ret = dbConn.GetRecordSet(sset);
    cout<<"ret "<<ret<<endl;
    nrow = sset.size();
    for(int i = 0;i<nrow; ++i)
    {
        int feildnum = sset[i].size();
        for(int j = 0;j<feildnum; ++j)
        {
            cout<<sset[i][j]<<"\t";
        }
        printf("\n------------------------------\n");
    }
    cout<<endl;
    return 0;
}


int main()
{
    CDataConn dbConn;
    const int LEN = 256;
    char buff[LEN]={};
    if(!dbConn.InitConn("81.68.68.181","root","hm123456","GYStudent",3306))
    {
        clog<<" conn faild "<<endl;
        return 1;
    }
    std::vector<std::vector<std::string> > dbset;

    for(int i = 0;i<5; ++i)
    {
        printf("leaf : %d \n",i);
        sprintf(buff, "select * from student limit %d, 5",i*5);
        if (dbConn.GetRecordSet(buff) == 0)
        {
            dbConn.PrintRecordSet();
        }
        dbConn.FreeResult();
        printf("\n-------------------------\n");
    }

    return 0;
}


int main()
{
    CDataConn dbConn;
    const int LEN = 256;
    char buff[LEN]={};
    if(!dbConn.InitConn("localhost","root","hm123456","GYStudent",3306))
    {
        clog<<" conn faild "<<endl;
        return 1;
    }

    for(int i = 0;i<10;++i)
    {
        sprintf(buff,"insert into student(sid,sname,ssex,sage) values('0900%d', 'dongwenhui', '%s' ,%d)",i+1,(rand()%2 ? "man":"woman"),rand()%10+15);

        printf("%s \n",buff);

        cout<<dbConn.ExecuteSQL(buff)<<endl;
    }

    return 0;

}

#include"../include/Timestamp.hpp"
#include"../include/Timer.hpp"
#include"../include/EvectionTimer.hpp"
#include"../include/ObjectPool3.hpp"
#include<thread>
#include<iostream>
using namespace std;
class Point
{
private:
    float _x;
    float _y;

public:
    Point() : _x(0.0), _y(0.0) {cout<<"Create Point()"<<endl;}
    Point(float x, float y) : _x(x), _y(y) {cout<<"Create Point(float,float)"<<endl;}
    Point(const Point &) = default;
    Point &operator=(const Point &) = default;
    ~Point() { cout<<"Destroy Point"<<endl;}
    float &pointX() { return _x; }
    const float &pointX() const { return _x; }

    float &pointY() { return _y; }
    const float &pointY() const { return _y; }

    void printpoint() const
    {
        std::this_thread::sleep_for(std::chrono::seconds(rand()%20));
        cout << _x << " : " << _y << endl;
    }
};

void funa(ObjectPool<Point> &objectPool)
{
    auto obj1 = objectPool.acquire(1,2);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj1) obj1->printpoint();

    auto obj2 = objectPool.acquire(3,4);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj2) obj2->printpoint();
    {
        auto obj3 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj3) obj3->printpoint();
        
        auto obj4 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj4) obj4->printpoint();
    }  
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;

}
void funb(ObjectPool<Point> &objectPool)
{
    auto obj1 = objectPool.acquire(1,2);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj1) obj1->printpoint();
    {
        auto obj2 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj2) obj2->printpoint();
        
        auto obj3 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj3) obj3->printpoint();
    }
    auto obj4 = objectPool.acquire(3,4);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj4) obj4->printpoint();
    
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;

}
int main()
{
    ObjectPool<Point>  objectPool(6,4,2,5);

    std::thread tha(funa,std::ref(objectPool));
    std::thread thb(funb,std::ref(objectPool));
    
    tha.join();
    thb.join();
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;
    // 等待一段时间，让驱逐策略生效
    std::this_thread::sleep_for(std::chrono::seconds(6));
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;
    return 0;
}


void funa()
{
    static int num = 0;
    cout<<"funa 2s "<<++num<<endl;
}
void funb()
{
    static int num = 0;
    cout<<"funb 6s "<<++num<<endl;
}



int main()
{
    Timer t1;
    Timer t2;
    t1.init();
    t1.set_timer(funa,2000);
    t2.init();
    t2.set_timer(funb,4000);
    while(1)
    {
        t2.handle_event();
        t1.handle_event();
    }
}

int main()
{
    
    EvectionTimer eveTimer;
    {
        Timer t1,t2;
        t1.init();
        t2.init();

        t1.set_timer(funa,2000);
        t2.set_timer(funb,6000);

        eveTimer.init(1000);
        eveTimer.add_timer(t1);
        eveTimer.add_timer(t2);
    }

    std::this_thread::sleep_for(std::chrono::seconds(60));
    eveTimer.set_stop();

}


class Point
{
private:
    float _x;
    float _y;

public:
    Point() : _x(0.0), _y(0.0) {cout<<"Create Point()"<<endl;}
    Point(float x, float y) : _x(x), _y(y) {cout<<"Create Point(float,float)"<<endl;}
    Point(const Point &) = default;
    Point &operator=(const Point &) = default;
    ~Point() { cout<<"Destroy Point"<<endl;}
    float &pointX() { return _x; }
    const float &pointX() const { return _x; }

    float &pointY() { return _y; }
    const float &pointY() const { return _y; }

    void printpoint() const
    {
        cout << _x << " : " << _y << endl;
    }
};

void funa(ObjectPool<Point> &objectPool)
{
    auto obj1 = objectPool.acquire(1,2);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj1) obj1->printpoint();

    auto obj2 = objectPool.acquire(3,4);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj2) obj2->printpoint();
    {
        auto obj3 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj3) obj3->printpoint();
        
        auto obj4 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj4) obj4->printpoint();
    }  
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;

}
void funb(ObjectPool<Point> &objectPool)
{
    auto obj1 = objectPool.acquire(1,2);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj1) obj1->printpoint();
    {
        auto obj2 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj2) obj2->printpoint();
        
        auto obj3 = objectPool.acquire(3,4);
        std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
        if(obj3) obj3->printpoint();
    }
    auto obj4 = objectPool.acquire(3,4);
    std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
    if(obj4) obj4->printpoint();
    
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;

}
int main()
{
    ObjectPool<Point>  objectPool(6,4,2,5);

    std::thread tha(funa,std::ref(objectPool));
    std::thread thb(funb,std::ref(objectPool));
    
    tha.join();
    thb.join();
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;
    // 等待一段时间，让驱逐策略生效
    std::this_thread::sleep_for(std::chrono::seconds(6));
    cout << "Total Object size:       " << objectPool.getTotalObjSize() << endl;
    cout << "Activate Object size:    " << objectPool.getActivateObjSize() << endl;
    cout << "Idle Object pool size:   " << objectPool.getIdleObjSize() << endl;
    return 0;
}



void func()
{
    static int num = 0;
    cout<<"func num:"<<++num<<endl;
}
void funa()
{
    static int num = 100;
    cout<<"funa num:"<<--num<<endl;
}

int main()
{
    Timer t1,t2;
    EvectionTimer evectime;
    t1.init();
    t1.set_timer(func,5000);
    t2.init();
    t2.set_timer(funa,2000);
    evectime.init();
    evectime.add_timer(t1);
    evectime.add_timer(t2);
    evectime.loop(1000);
}


int main()
{
    Timestamp tp;
    cout<<tp.toString()<<endl;
    cout<<tp.toFormattedString()<<endl;
    tp.now();
    cout<<tp.toString()<<endl;
    cout<<tp.toFormattedString()<<endl;
    tp = tp + 120;
    cout<<tp.toFormattedString()<<endl;
    return 0;
}
#endif
