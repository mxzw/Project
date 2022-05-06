#include <unistd.h>

#include <jsoncpp/json/json.h>

#include <iostream>
#include <string>
#include <list>

#include "ManageDB.hpp"
#include "./xpack-master/json.h"


using namespace std;

struct UserStudyMessage
{
  UserStudyMessage(int sid=0,string stitle="",string sbody=0,int uid=0)
    :study_id(sid),study_title(stitle),study_body(sbody),user_id(uid)
  {}
  int study_id;
  string study_title;
  string study_body;
  int user_id;
  XPACK(O(study_id,study_title,study_body,user_id));
};

class UserStudy{
  public:
    UserStudy(){}
    ~UserStudy(){}

    //查询所有的文章信息
    string UserStudyMessageQuery(ManageDB*& md_)
    {
      // 将文章的信息存储在链表中                     
      list<UserStudyMessage> lcm;                     
      lcm.clear();                     
      //统计文章信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define UserStudyMessageQuery_SQL "select * from UserStudyInfo;"
      snprintf(sql,sizeof(sql)-1,UserStudyMessageQuery_SQL);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : UserStudyMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int study_id = atoi(row[0]);                                                                                          
        string study_title(row[1]);
        string study_body = string(row[2]);
        int user_id = atoi(row[3]);

        UserStudyMessage cm(study_id,study_title,study_body,user_id);
        lcm.push_back(cm);
        //统计数据的个数
        countNum++;
      }
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(lcm);

      string prev = "{\"count\":"+ to_string(countNum) +",\"data\":";
      str = prev + str + "}";  
      return str;
    }
    //查询所有的文章信息
    string UserStudyMessageQuery(ManageDB*& md_,int uid)
    {
      // 将文章的信息存储在链表中                     
      list<UserStudyMessage> lcm;                     
      lcm.clear();                     
      //统计文章信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define UserStudyMessageQuery2_SQL "select * from UserStudyInfo where user_id=%d;"
      snprintf(sql,sizeof(sql)-1,UserStudyMessageQuery2_SQL,uid);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : UserStudyMessageQuery2, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int study_id = atoi(row[0]);                                                                                          
        string study_title(row[1]);
        string study_body = string(row[2]);
        int user_id = atoi(row[3]);

        UserStudyMessage cm(study_id,study_title,study_body,user_id);
        lcm.push_back(cm);
        //统计数据的个数
        countNum++;
      }
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(lcm);

      string prev = "{\"count\":"+ to_string(countNum) +",\"data\":";
      str = prev + str + "}";  
      return str;
    }

    //根据文章名来模糊搜索文章信息
    string UserStudyMessageSearch(ManageDB*& md_,string study_title)
    {
      // 将文章的信息存储在链表中                     
      list<UserStudyMessage> lcm;                     
      lcm.clear();                     
      //统计文章信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define UserStudyMessageSearch_SQL "select * from UserStudyInfo where study_title like \'%%%s%%\';"
      snprintf(sql,sizeof(sql)-1,UserStudyMessageSearch_SQL,study_title.c_str());
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : UserStudyMessageSeach, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int study_id = atoi(row[0]);                                                                                          
        string study_title(row[1]);
        string study_body = string(row[2]);
        int user_id = atoi(row[3]);

        UserStudyMessage cm(study_id,study_title,study_body,user_id);
        lcm.push_back(cm);
        //统计数据的个数
        countNum++;
      }
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(lcm);

      string prev = "{\"count\":"+ to_string(countNum) +",\"data\":";
      str = prev + str + "}";  
      return str;
    }

    //新增文章信息
    bool AddUserStudyMessage(ManageDB*& md_,Json::Value& v)    
    {    
      string study_title = v["study_title"].asString();                         
      string study_body = v["study_body"].asString();    
      int user_id = stoi(v["user_id"].asString());
    
      char sql[1024]={0};         
#define AddUserStudyMessage_SQL "insert into UserStudyInfo values(null,\'%s\',\'%s\',%d);"
      snprintf(sql,sizeof(sql)-1,AddUserStudyMessage_SQL,study_title.c_str(),study_body.c_str(),user_id);    
    
      return md_->ExecSQL(sql);     
    } 
    // 根据文章id查询文章信息
    string IdToUserStudyMessage(ManageDB*& md_,int tid)
    {
      char sql[1024]={0};
#define IdToUserStudyMessage_SQL "select * from UserStudyInfo where study_id = %d;"
      snprintf(sql,sizeof(sql)-1,IdToUserStudyMessage_SQL,tid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : IdToUserStudyMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      int study_id = atoi(row[0]);                                                                                          
      string study_title(row[1]);
      string study_body(row[2]);
      int user_id = atoi(row[3]);

      UserStudyMessage cm(study_id,study_title,study_body,user_id);
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(cm);
      return str;
    }
    //修改文章信息
    bool UpdateUserStudyMessage(ManageDB*& md_,Json::Value& v)
    {
      int study_id = stoi(v["study_id"].asString());
      string study_title = v["study_title"].asString();                         
      string study_body = v["study_body"].asString();
      int user_id = stoi(v["user_id"].asString());

      char sql[1024]={0};
#define UpdateUserStudyMessage_SQL "update UserStudyInfo set study_title =\'%s\',study_body=\'%s\',user_id=%d where study_id=%d;"
      snprintf(sql,sizeof(sql)-1,UpdateUserStudyMessage_SQL,study_title.c_str(),study_body.c_str(),user_id,study_id);
      return md_->ExecSQL(sql);
    }

    //删除文章信息
    bool DelUserStudyMessage(ManageDB*& md_,Json::Value& v)
    {
      int study_id = stoi(v["study_id"].asString());
      char sql[1024]={0};
#define DelUserStudyMessage_SQL "delete from UserStudyInfo where study_id=%d;"
      snprintf(sql,sizeof(sql)-1,DelUserStudyMessage_SQL,study_id);
      return md_->ExecSQL(sql);
    }

};

