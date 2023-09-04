#include "../include/CDConnPool.hpp"


// KeyedObjectPool<std::string,CDataConn> connpool;
// const std::string &m_host;
// const std::string &m_user;
// const std::string &m_passwd;
// const std::string &m_db;
// unsigned int m_port;

CDConnPool::CDConnPool(const std::string &host,
           const std::string &user,
           const std::string &passwd,
           const std::string &db,
           unsigned int port)
    :m_host(host),
    m_user(user),
    m_passwd(passwd),
    m_db(db),
    m_port(port),
    connpool(16,16,8,60)
{
}

int CDConnPool::ExecuteSQL(const std::string &sql)
{
    int ret = 0;
    auto pa = connpool.acquire("Execute",m_host,m_user,m_passwd,m_db,m_port);
    if(pa)
    {
        ret = pa->ExecuteSQL(sql);
    }
    return ret;
}

int CDConnPool::GetRecordSet(const std::string &sql,
           std::vector<std::vector<std::string>> &sset)
{
    int ret = 0;
    auto pa = connpool.acquire("Select",m_host,m_user,m_passwd,m_db,m_port);
    if(pa)
    {
        ret = pa->GetRecordSet(sql);
        if(0 == ret)
        {
            pa->GetRecordSet(sset);
            pa->FreeResult();
        }
    }
    return ret;
}