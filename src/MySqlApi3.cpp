#include"../include/MySqlApi3.hpp"
// MYSQL mysqlConn;
// MYSQL *mysql;
// MYSQL_RES *recordset;

CDataConn::CDataConn()
    :mysqlConn{},mysql(nullptr),recordset(nullptr)
{
    if(mysql_library_init(0,nullptr,nullptr) != 0)
    {
        clog<<"mysql library int failed"<<endl;
        exit(EXIT_FAILURE);
    }
    if((mysql = mysql_init(&mysqlConn)) == nullptr)
    {
        clog<<"mysql_init failed "<<endl;
        exit(EXIT_FAILURE);
    }
}
CDataConn::CDataConn(const std::string &host,
                    const std::string &user,
                    const std::string &passwd,
                    const std::string &db,
                    unsigned int port)
    :CDataConn() // c++11; 委托构造函数
{
    if(!InitConn(host,user,passwd,db,port))
    {
        exit(EXIT_FAILURE);
    }
}
CDataConn::~CDataConn()
{
    ExitConnect();
    mysql_library_end(); //mysql_library_init(0,nullptr,nullptr)
}

bool CDataConn::InitConn(const std::string &host,
              const std::string &user,
              const std::string &passwd,
              const std::string &db,
              unsigned int port)
{
    bool res = true;
    if(mysql_real_connect(mysql,
                          host.c_str(),
                          user.c_str(),
                          passwd.c_str(),
                          db.c_str(),
                          port,
                          nullptr,0) == nullptr)
    {
        clog<<"failed to connect to database :"<<db<<endl;
        cout<<mysql_error(mysql)<<endl;
        res = false;
    }
    return res;
}

void CDataConn::ExitConnect()
{
    mysql_close(mysql); //mysql_init(&mysqlConn)
    mysql = nullptr;
}
// insert update delete; create table; drop table;
int CDataConn::ExecuteSQL(const std::string &sql)
{ // sql =>"select * from Student";
    int res = 0;
    if(sql.find("select") == 0)
    {
        clog<<"not insert updata delete create drop "<<endl;
        return -1;
    }
    //mysql_real_query 函数执行成功返回0， 否则返回非0值
    //if(res = mysql_query(mysql,sql.c_str()))
    if(res = mysql_real_query(mysql,sql.c_str(),sql.size()))
    {
       // clog<<"could not execute statements "<<res<<endl;
        clog<<mysql_error(mysql)<<endl;
        return -2;
    }
    return mysql_affected_rows(mysql);
}

// GetRecordSet("select * from");
int CDataConn::GetRecordSet(std::vector<std::vector<std::string> > &strset)
{
    strset.clear();
    recordset = mysql_store_result(mysql); //获取结果,结果集
    if(nullptr == recordset) return -1;
    int rownum = mysql_num_rows(recordset);
    int fieldnum = mysql_field_count(mysql);
    strset.resize(rownum);
    for(int i = 0;i < rownum; ++i)
    {
        strset[i].reserve(fieldnum);
        char **rs = mysql_fetch_row(recordset);
        for(int j = 0;j < fieldnum; ++j)
        {
            strset[i].emplace_back(rs[j]);
        }
    }
    return 0;
}
//FreeResult()
// "select * from student where sage >= 12;
int CDataConn::GetRecordSet(const std::string &sql)
{
    if(sql.find("select") != 0)
    {
        clog<<"not select ..."<<endl;
        return -1;
    }
    //if(mysql_query(mysql,sql.c_str()));
    if(mysql_real_query(mysql,sql.c_str(),sql.size()) != 0)
    {
        clog<<mysql_error(mysql)<<endl;
        return -2;
    }
    return 0;
}
void CDataConn::FreeResult()
{
    if(recordset != nullptr)
    {
        mysql_free_result(recordset);
    }
    recordset = nullptr;
}

void CDataConn::PrintRecordSet() 
{
    // GetRecordSet
    recordset = mysql_store_result(mysql); //获取结果,结果集
    if(nullptr == recordset) return ;
    int rownum = mysql_num_rows(recordset);
    int fieldnum = mysql_field_count(mysql);
    for(int i = 0;i<rownum;++i)
    {
        char **rs = mysql_fetch_row(recordset);
        for(int j = 0;j<fieldnum;++j)
        {
            printf("%s \t",rs[j]);
        }
        printf("\n");
    }
    printf("\n");
    

}

