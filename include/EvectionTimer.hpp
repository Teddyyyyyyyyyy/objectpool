#ifndef EVECTIONTIMER_HPP
#define EVECTIONTIMER_HPP
#include<unordered_map>
#include"Timer.hpp"
#include<vector>
#include<sys/epoll.h>
#include<thread>
class EvectionTimer
{
private:
   int m_epollfd;
   bool m_stop = true;//是否停止
   std::unordered_map<int,Timer> timers;
   std::vector<epoll_event> m_events;
   static const int init_event = 16;
   std::thread word_thread;
public:
   EvectionTimer();
   ~EvectionTimer();

   EvectionTimer(const EvectionTimer &) = delete;
   EvectionTimer & operator=(const EvectionTimer&) = delete;

   bool init(size_t );
   void add_timer(Timer &t);
   void remove_timer(Timer &t);
   void loop(int timeout);
   void set_stop();
};
#endif