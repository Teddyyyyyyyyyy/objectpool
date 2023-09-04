
#ifndef TIMER_HPP
#define TIMER_HPP
#include<functional>
using namespace std;
class Timer
{
public:
    using TimerCallback = std::function<void(void)>;//bind;
private:
    int m_fd;//定时器描述符
    TimerCallback m_callback;
    bool settime(size_t interval);//毫秒
public:
    Timer();
    ~Timer();
    Timer(const Timer &) = delete;
    Timer & operator=(const Timer &) = delete;
    Timer(Timer &&);
    Timer & operator=(Timer &&);

    bool init();
    bool set_timer(const TimerCallback &cb,size_t interval);//ms

    bool reset_timer(size_t interval);
    bool deleter_timer();
    void handle_event();
    int get_fd()const;
};


#endif