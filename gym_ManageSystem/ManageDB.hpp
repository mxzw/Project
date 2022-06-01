#pragma once

#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


class ManageDB{
  public:
    ManageDB()
    {
      mysql_init(&mq_);
    }

    ~ManageDB()
    {
      mysql_close(&mq_);
    }

    // 连接数据库，采用mysql_real_connect（）函数
    bool ConnectDB()
    {
      const char* host ="192.168.13.129";
      const char* user = "zw";
      const char* passwd = "123456";
      const char* db = "GymManageSys";
      int port = 3306;

      if(!mysql_real_connect(&mq_,host,user,passwd,db,port,NULL,CLIENT_FOUND_ROWS))
      {
        return false;
      }
      return true;
    }


    //获取用户当前属性（管理员/用户）
    int GetUserAttributes(int user_id)
    {
      char sql[1024] = {0};
#define GET_USER_ATTR "select user_attributes from LoginUserInfo where user_id=%d;"
      snprintf(sql,sizeof(sql)-1,GET_USER_ATTR,user_id);

      MYSQL_RES* res;
      if(!ExecSQL(sql,res))
        return -1;

      if(mysql_num_rows(res) != 1)
      {
        cout << "No data : sql is " << sql << endl;
        return -2;
      }

      MYSQL_ROW row = mysql_fetch_row(res);
#define JUDGE_VALUE(p) ((p != NULL) ? p : "")

      mysql_free_result(res);
      return atoi(row[0]);

    }
    //修改密码
    bool ModifyPasswd(Json::Value& v,int user_id)
    {
     string newPasswd = v["newPasswd"].asString();
     string oldPasswd = v["oldPasswd"].asString();
     if(newPasswd.empty())
       return -1;
     char sql[1024] ={};
#define Verify_OldP "select user_passwd from LoginUserInfo where user_id=%d;"
      snprintf(sql,sizeof(sql)-1,Verify_OldP,user_id);

      MYSQL_RES* res;
      if(!ExecSQL(sql,res) || mysql_num_rows(res) != 1)
        return false;


      MYSQL_ROW row = mysql_fetch_row(res);
#define JUDGE_VALUE(p) ((p != NULL) ? p : "")
      string pw = JUDGE_VALUE(row[0]);

      if(pw != oldPasswd)
      {	
        cout << "原密码不一致" << endl;
        return false;
      }
      mysql_free_result(res);

      memset(sql,0,sizeof(sql));
#define ModifyP_SQL "update LoginUserInfo set user_passwd=\'%s\' where user_id=%d;"
      snprintf(sql,sizeof(sql)-1,ModifyP_SQL,newPasswd.c_str(),user_id);

      return ExecSQL(sql);

    }

    //将用户信息添加到用户表中
    int AddUser(Json::Value& v)
    {
      //1.组织sql语句
      string name = v["register_user"].asString();
      string email = v["register_email"].asString();
      string passwd = v["register_passwd"].asString();
      int attri = 2;

      if(name.empty() || email.empty() || passwd.empty())
        return -1;

      char sql[1024];
#define GET_USER_INFO "select * from LoginUserInfo where user_email=\'%s\';"
      snprintf(sql,sizeof(sql)-1,GET_USER_INFO,email.c_str());

      MYSQL_RES* res;
      if(!ExecSQL(sql,res))
        return -1;

      if(mysql_num_rows(res) >= 1)
      {
        cout << "User is Exist " << sql << endl;
        return -2;
      }
      memset(sql,0,sizeof(sql));
#define ADD_USER_INFO "insert into LoginUserInfo(user_name,user_email,user_passwd,user_attributes) values(\'%s\',\'%s\',\'%s\',%d);"
      snprintf(sql,sizeof(sql)-1,ADD_USER_INFO,name.c_str(),email.c_str(),passwd.c_str(),attri);
      mysql_free_result(res);

      return ExecSQL(sql) == true ? 1 : -1;
    }

    //检查用户是否存在
    int CheckUserIsExist(Json::Value& v)
    {
      string email = v["login_email"].asString();
      string passwd = v["login_passwd"].asString();

      char sql[1024] = {0};
#define GET_USER_INFO "select * from LoginUserInfo where user_email=\'%s\';"
      snprintf(sql,sizeof(sql)-1,GET_USER_INFO,email.c_str());

      MYSQL_RES* res;
      if(!ExecSQL(sql,res))
        return -1;

      if(mysql_num_rows(res) != 1)
      {
        cout << "No data : sql is " << sql << endl;
        return -2;
      }

      MYSQL_ROW row = mysql_fetch_row(res);
#define JUDGE_VALUE(p) ((p != NULL) ? p : "")
      string pw = JUDGE_VALUE(row[3]);

      if(pw != passwd)
      {	
        return -3;
      }

      //释放结果集并返回user_id
      mysql_free_result(res);

      return atoi(row[0]);
    }

    //将sql语句送入数据库中去处理,并且接收数据库返回的结果集
    bool ExecSQL(const string& sql,MYSQL_RES*& res)
    {
      //设置当前字符集
      mysql_query(&mq_,"set names \'utf8\'");
      //解决从结果集中读取中文数据显示乱码问题
      mysql_query(&mq_,"set character_set_results=utf8");
      mysql_query(&mq_,"set character set utf8");

      //等于0就成功，不等于0就失败
      if(mysql_query(&mq_,sql.c_str())!=0)
      {
        cout << "ExecSQL failed,sql is " << sql << endl;
        return false;
      }


      //mysql执行的结果均是放在结果集中
      //MYSQL_RES * STDCALL mysql_store_result(MYSQL *mysql);

      res = mysql_store_result(&mq_);
      if(!res)
        return false;
      return true;

    }

    //重载，只不过不保存结果集
    bool ExecSQL(const string& sql)
    {
      //设置当前字符集
      mysql_query(&mq_,"set names \'utf8\'");
      //解决从结果集中读取中文数据显示乱码问题
      mysql_query(&mq_,"set character_set_results=utf8");
      mysql_query(&mq_,"set character set utf8");

      //等于0就成功，不等于0就失败
      if(mysql_query(&mq_,sql.c_str())!=0)
      {
        cout << "ExecSQL failed, sql is " << sql << endl;
        return false;
      }
      return true;
    }


  private:
    MYSQL mq_;
};
