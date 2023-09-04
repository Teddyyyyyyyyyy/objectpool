#include"../include/Timestamp.hpp"
#include<sys/time.h>
#include<time.h>
  //std::int64_t mesc_;//微秒
 
  Timestamp::Timestamp():msec_(0){}
  Timestamp::Timestamp(std::int64_t ms):msec_(ms){}
  Timestamp::~Timestamp()
  {
    //cout<<"~Timestamp"<<endl;
  }
  Timestamp Timestamp:: now()//当前时间
  {
    *this = Timestamp::Now();
    return *this;
  }
  void Timestamp::swap(Timestamp &ts)
  {
    std::swap(this->msec_,ts.msec_);
  }
  std::string Timestamp::toString()const//将时间戳转为字符串
  {
    char buff[BUFFLEN] = {0};
    std::int64_t seconds = msec_ / KMSP;//秒
    std::int64_t microseconds = msec_ % KMSP;//微秒
    snprintf(buff,BUFFLEN,"%ld.%06ld",seconds,microseconds);
    return std::string(buff);
  }

  std::string Timestamp::toFormattedString(bool showms)const//是否显示微秒
  {
    char buff[BUFFLEN] = {0};
    std::int64_t seconds = msec_ / KMSP;//秒
    std::int64_t microseconds = msec_ % KMSP;//微秒
    struct tm tm_time;
    //gmtime_r(&seconds,&tm_time);格林威治标准时间
    localtime_r(&seconds,&tm_time);
    int pos = snprintf(buff,BUFFLEN,"%4d/%02d/%02d-%02d:%02d:%02d",
    tm_time.tm_yday + 1900,tm_time.tm_mon+1,tm_time.tm_mday,tm_time.tm_hour,
    tm_time.tm_min,tm_time.tm_sec);
    if(showms)
    {
        snprintf(buff+pos,BUFFLEN - pos,".%ldZ",microseconds);
    }
    return std::string(buff);
  }

  Timestamp Timestamp::Now()//得到当前时间
  {
    struct timeval tv = {};
    gettimeofday(&tv,nullptr);
    std::int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * KMSP + tv.tv_usec);
  }
  Timestamp Timestamp::invalid()//得到无效时间
  {
    return Timestamp();
  }
  bool Timestamp::operator<(const Timestamp &other) const
  {
    return this->msec_ < other.msec_;
  }
  bool Timestamp::operator==(const Timestamp &other) const
  {
    return this->msec_ == other.msec_;
  }
  bool Timestamp::operator!=(const Timestamp &other) const
  {
    return !(*this == other);
  }
  std::int64_t Timestamp::operator-(const Timestamp &other) const//秒
  {
    std::int64_t diff = this->msec_ - other.msec_;
    return diff/KMSP;
  }
  Timestamp Timestamp::operator+(const std::int64_t seconds) const//时间戳和秒相加
  {
    return Timestamp(this->msec_ + seconds*KMSP);
  }
  std::int64_t Timestamp::diffmse(const Timestamp &t)const
  {
    return this->msec_ - t.msec_;
  }

