#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP
#include<iostream>
#include<string>
using namespace std;
class Timestamp
{
private:
  std::int64_t msec_;//微秒
  static const int KMSP = 1000*1000; //秒
  static const size_t BUFFLEN = 128;//缓冲区
public:
  Timestamp();
  explicit Timestamp(std::int64_t ms);
  ~Timestamp();
  Timestamp now();//当前时间
  void swap(Timestamp &ts);
  std::string toString() const;//将时间戳转为字符串
  std::string toFormattedString(bool showms = true)const;//是否显示微秒
public:
  static Timestamp Now();//得到当前时间
  static Timestamp invalid();//得到无效时间
  bool operator<(const Timestamp &other) const;
  bool operator==(const Timestamp &other) const;
  bool operator!=(const Timestamp &other) const;
  std::int64_t operator-(const Timestamp &other) const;//秒
  Timestamp operator+(const std::int64_t seconds) const;//时间戳和秒相加
  std::int64_t diffmse(const Timestamp &other) const;
};

#endif