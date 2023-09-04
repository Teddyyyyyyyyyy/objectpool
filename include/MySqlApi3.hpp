#ifndef MYSQLAPI3_HPP
#define MYSQLAPI3_HPP
#include<mysql/mysql.h>
#include<iostream>
#include<string>
#include<vector>
using namespace std;

class CDataConn
{
private:
    MYSQL mysqlConn;
    MYSQL *mysql;
    MYSQL_RES *recordset;
public:
    CDataConn();
    CDataConn(const std::string &host,
              const std::string &user,
              const std::string &passwd,
              const std::string &db,
              unsigned int port);
    ~CDataConn();

    CDataConn(const CDataConn&) = delete;
    CDataConn & operator=(const CDataConn &) = delete;
    
public:
    bool InitConn(const std::string &host,
              const std::string &user,
              const std::string &passwd,
              const std::string &db,
              unsigned int port);
    

    void ExitConnect();
    // insert update delete; create table; drop table;
    int ExecuteSQL(const std::string &sql);
    
    // select ;
    int GetRecordSet(const std::string &sql);
    void FreeResult();
    void PrintRecordSet();

    int GetRecordSet(std::vector<std::vector<std::string> > &slectset);
};
#endif 