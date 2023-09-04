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

    mysql_close(mysql);
    mysql = nullptr;
}