#pragma once
#include <iostream>
#include <string>

#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#include "room_player.hpp"

using namespace std;


class ManagerDB{

    public:
        ManagerDB()
        {
            mysql_init(&ms_);
        }

        ~ManagerDB()
        {
            mysql_close(&ms_);
        }

        bool ConnectDB()
        {
           /* MYSQL * STDCALL mysql_real_connect(
            *   MYSQL *mysql,  操作句柄
            *   const char *host,  登录ip
            *   const char *user,  用户名
            *   const char *passwd, 密码
            *   const char *db,     数据库
            *   unsigned int port,   端口  
            *   const char *unix_socket, 本地域套接字（是否在本地进行）                       
            *   unsigned long clientflag); 
            */
            const char *host = "118.89.67.215";
            const char *user = "zw";
            const char *passwd = "1476950779";
            const char *db = "webgobang";
            int  port = 3306;
            
            if(!mysql_real_connect(&ms_,host,user,passwd,db,port,NULL,CLIENT_FOUND_ROWS))
            {
                return false;
            }
            return true;
        }

        /*
         * AddUser : 添加用户到数据库中
         * 传入Json串
         * 成功：true
         * 失败：false
         */ 
        bool AddUser(Json::Value& v)
        {
            //1.组织sql语句
            string name = v["name"].asString();
            string email = v["email"].asString();
            string phone_num = v["phonenum"].asString();
            string password = v["passwd"].asString();
            if(name.empty() || email.empty() || phone_num.empty() || password.empty())
                return false;
            char sql[1024] = {0};
//定义一个宏来表示SQL语句            
#define ADD_USER_INFO "insert into sys_user(name,email,phone_num,passwd) values(\'%s\',\'%s\',\'%s\',\'%s\');"
            snprintf(sql,sizeof(sql)-1,ADD_USER_INFO,name.c_str(),email.c_str(),phone_num.c_str(),password.c_str());

            return ExecSQL(sql);

        }

        /*
         * CheckUserExist ： 检查用户是否存在
         * 传入对应的Json串
         *  成功：返回user_id
         *  失败：返回负数
         */ 
        int CheckUserExist(Json::Value& v)
        {
            //1.组织sql语句
            string email = v["email"].asString();
            string password = v["password"].asString();
            //string tmp = "select user_id from sys_user where email = \"%s\" and password = \"%s\"";
            char sql[1024] = {0};
//定义一个宏来表示SQL语句            
#define GET_USER_INFO "select * from sys_user where email = \'%s\';"
            snprintf(sql,sizeof(sql)-1,GET_USER_INFO,email.c_str());
            
            //2.执行在数据库
            MYSQL_RES* res;
            if(!ExecSQL(sql,res))
                return -2;
            
            //3.将查询到的密码和传进来的密码进行对比
            //3.1 首先对当前结果行进行判读，如果查询到的结果行数不为1，说明当前用户是不存在的(或存在异常情况)
            //          my_ulonglong STDCALL mysql_num_rows(MYSQL_RES *res); 
            if(mysql_num_rows(res) != 1)
            {
                cout << "No data ：sql is  " << sql << endl;
                return -3;
            }

            //3.2然后再获取该结果集的中这一行
            //          MYSQL_ROW   STDCALL mysql_fetch_row(MYSQL_RES *result);
			MYSQL_ROW row = mysql_fetch_row(res);
			//row[4] ：对应着当前行中passwd
#define JUDGE_VALUE(p) ((p != NULL) ? p : "")
			string pw = JUDGE_VALUE(row[4]);
			
			if(pw != password)
			{	
				return -4;
			}

            //释放结果集并返回user_id
            mysql_free_result(res);

            return atoi(row[0]);
        }

        /*
         * ExecSQL ：执行相应的SQL语句
         * 传入SQL语句，以及想要报存的结果集
         * 成功：true
         * 失败：false 
         * int STDCALL mysql_query(MYSQL *mysql, const char *q); 
         * */
        bool ExecSQL(const string& sql,MYSQL_RES*& res)
        {
            //设置当前字符集
            mysql_query(&ms_,"set name utf8");

            //等于0表示成功，不等于0表示失败
            int ret = mysql_query(&ms_,sql.c_str());
            if(ret != 0)
                return false;

            //mysql执行的结果均是放在结果集中
            //MYSQL_RES * STDCALL mysql_store_result(MYSQL *mysql);
            
            res = mysql_store_result(&ms_);
            if(!res)
                return false;

            return true;
        }

        /*
         * ExecSQL ： 重载，只是实现了不接受结果集而已
         */ 
        bool ExecSQL(const string& sql)
        {
            //设置当前字符集
            mysql_query(&ms_,"set name utf8");

            //等于0表示成功，不等于0表示失败
            int ret = mysql_query(&ms_,sql.c_str());
            if(ret != 0)
                return false;
            return true;

        }

        /*
         * InsertRoomInfo : 在数据库中插入当前对局信息
         * 为了防止在多次插入数据的时候，某条数据插入错误，导致对局信息错误
         * 我们应该：
         *  1.开启事务
         *  2.创建保存点
         *  当发送错误数据的时候
         *    1.回滚/提交事务
         *  3.提交事务
         */  
        int InsertRoomInfo(Room& r)
        {
           //想要插入对局当中所有的落子信息,需要使用到事务
            //开启事务
            if(ExecSQL("start transaction;") == false)
            {
                //开启事务失败
                return -1;
            }
            //创建保存点
            if(ExecSQL("savepoint start_insert;") == false)
            {
                return -2;
            }
            
#define INSERT_ROOM_INFO "insert into game_info values(%d,%d,%d,%d,%d,\'%s\');"
           vector<string> sv = r.GetRoomStepInfo();
           for(size_t i = 0; i < sv.size(); ++i)
           {
               char sql[1024] = {0};
               snprintf(sql,sizeof(sql)-1,INSERT_ROOM_INFO,r.room_id_,r.p1_,r.p2_,r.winner_,r.p1_,sv[i].c_str());

               //当插入失败的时候，回滚/提交事务
               if(ExecSQL(sql)==false)
               {
                   //回滚
                   ExecSQL("rollback to start_insert;");
                   //提交事务
                   ExecSQL("commit;");
                   return -3;
               }
           }
           ExecSQL("commit;");
           return 0;
        }

    private:
        MYSQL ms_;
};
