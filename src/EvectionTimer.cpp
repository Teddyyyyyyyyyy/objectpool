#include"../include/EvectionTimer.hpp"
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<thread>
using namespace std;
//int m_epollfd;
//bool m_stop = false;//是否停止
//std::unordered_map<int,Timer> timers;
//std::vector<epoll_event> m_events;
//static const int init_event = 16;

EvectionTimer::EvectionTimer():m_epollfd(-1){m_events.resize(init_event);}
EvectionTimer::~EvectionTimer(){set_stop();//cout<<"~EvectionTimer"<<endl; 
}
bool EvectionTimer::init(size_t timeout)
{
    bool res = false;
    m_stop = false;
    m_epollfd = epoll_create1(EPOLL_CLOEXEC);
    if(m_epollfd > 0)
    {
       try
       {
        word_thread = std::thread(&EvectionTimer::loop,this,timeout);
        res = true;
       }
       catch(const std::exception& e)
       {
        std::cerr << e.what() << '\n';
        close(m_epollfd);
       }
    }
    return res;
}
void EvectionTimer::add_timer(Timer &t)//局部定时器析构
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = t.get_fd();
    if(epoll_ctl(m_epollfd,EPOLL_CTL_ADD,t.get_fd(),&ev) < 0)
    {
        fprintf(stderr,"epoll_ctl EPOLL_CTL_ADD failed,err = %s\n",strerror(errno));
        return;
    }
    timers[t.get_fd()] = std::move(t);
}
//
void EvectionTimer::remove_timer(Timer &t)//epollzhongmeiyouhuibuhuibaozuo
{
    epoll_ctl(m_epollfd,EPOLL_CTL_DEL,t.get_fd(),nullptr);
    timers.erase(t.get_fd());
}

void EvectionTimer::loop(int timeout)//毫秒
{
    while(!m_stop)
    {
        //std::cout<<"loop:..."<<timeout<<std::endl;
        int n = epoll_wait(m_epollfd,m_events.data(),m_events.size(),timeout);
        //cout<<"epoll_wait:n"<<n<<endl;
        for(int i = 0;i<n;++i)
        {
            int fd = m_events[i].data.fd;
            std::unordered_map<int,Timer>::iterator it = timers.find(fd);
            if(it != timers.end())
            {
                //cout <<"回调函数"<<endl;
                auto &t = it->second;
                t.handle_event();
            }
        }
        if(n >= m_events.size())
        {
            m_events.resize(m_events.size()*2);
        }
    }
}
void EvectionTimer::set_stop()
{
    if(m_stop) return;
    m_stop = true;
    if(word_thread.joinable())
    {
        word_thread.join();
    }
    close(m_epollfd);
    m_epollfd = -1;
}
