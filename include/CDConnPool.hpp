#ifndef CDCONNPOOL_HPP
#define CDCONNPOOL_HPP

#include"ObjectPool3.hpp"
#include"KeyedObjectPool.hpp"
#include"MySqlApi3.hpp"
#include<vector>
#include<string>
using namespace std;

class CDConnPool
{
private:
    KeyedObjectPool<std::string,CDataConn> connpool;

    const std::string &m_host;
    const std::string &m_user;
    const std::string &m_passwd;
    const std::string &m_db;
    unsigned int m_port;
public:
    CDConnPool(const std::string &host,
              const std::string &user,
              const std::string &passwd,
              const std::string &db,
              unsigned int port);

    int ExecuteSQL(const std::string &sql);
    // select ;
    int GetRecordSet(const std::string &sql,std::vector<std::vector<std::string>> &sset);
};

#endif 