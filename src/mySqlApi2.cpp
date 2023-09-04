#include<mysql/mysql.h>
#include<string>
#include<iostream>
using namespace std;

int main()
{
    MYSQL mysqlconn;
    MYSQL *mysql = mysql_init(&mysqlconn);
    if(nullptr == mysql)
    {
        printf("mysql init error \n");
        return 1;
    }
    printf("mysqconn %p : %p",mysql,&mysqlconn);
    //
    if(mysql_real_connect(mysql,
                    "localhost",
                    "root",
                    "123456",
                    "c2205db",3306,nullptr,0) == nullptr)
    {
        printf("connection error \n");
        mysql_close(mysql);
        return 1;
    }
    printf("conn ..\n");
    const char *sqlstr = "create table student(sid varchar(10),sname varchar(20),ssex varchar(4),sage int)";
    if(mysql_query(mysql,sqlstr))
    {
        printf("create table error \n");
    }//成功 0  
    //int len = mysql_read_query(mysql,sqlstr,strlen(sqlstr));
    mysql_close(mysql);
    mysql = nullptr;
}