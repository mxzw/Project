#pragma once
#include <unistd.h>
#include <jsoncpp/json/json.h>

#include <iostream>
#include <string>
#include <list>

#include "ManageDB.hpp"
#include "Date.hpp"
#include "./xpack-master/json.h"

using namespace std;


struct CoachMessage
{
  CoachMessage(int cid=0,string cname="",string cphone="",int csex=-1,int cage=0,string cdate="",int ceduage=0,double csalary=0,int cstate=0)
    :coach_id(cid),coach_name(cname),coach_phone(cphone),coach_sex(csex),coach_age(cage),coach_date(cdate),coach_educateAge(ceduage),coach_salary(csalary),coach_state(cstate)
  {}
  int coach_id;
  string coach_name;
  string coach_phone;
  int coach_sex;
  int coach_age;
  string coach_date;
  int coach_educateAge;
  double coach_salary;
  int coach_state;
  XPACK(O(coach_id,coach_name,coach_phone,coach_sex,coach_age,coach_date,coach_educateAge,coach_salary,coach_state));
};

class Coach{
  public:
    Coach(){}
    ~Coach(){}
    string CoachMessageQuery(ManageDB*& md_)
    {
      // 将用户的信息存储在链表中                     
      list<CoachMessage> lcm;                     
      lcm.clear();                     
      //统计用户信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define CoachMessageQuery_SQL "select * from CoachInfo;"
      snprintf(sql,sizeof(sql)-1,CoachMessageQuery_SQL);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : CoachMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int coach_id = atoi(row[0]);                                                                                          
        string coach_name(row[1]);
        string coach_phone(row[2]);
        int coach_sex = atoi(row[3]);                   
        int coach_age = atoi(row[4]);                     
        string coach_date(row[5]);
        int coach_educateAge = atoi(row[6]);                  
        double coach_salary = stod(string(row[7]));
        int coach_state = atoi(row[8]);

        //将登录时间初始化为Data格式
        Date date(coach_date);
        //加上yay-mm-dd 中的0 ，比如2021-2-1 --> 2021-02-01,目的在于统一时间的显示
        coach_date = date.ToString();


        CoachMessage cm(coach_id,coach_name,coach_phone,coach_sex,coach_age,coach_date,coach_educateAge,coach_salary,coach_state);
        lcm.push_back(cm);
        //统计数据的个数
        countNum++;
      }
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(lcm);

      //开始组织json数据，即发送给bootstrapTable使用的json数据    
      //client模式：{"count":xxx,"data":[{},{},{}]}                                                                                                                   
      //server模式：{"total":xxx,"rows":[{},{},{}]}
      string prev = "{\"count\":"+ to_string(countNum) +",\"data\":";
      str = prev + str + "}";  
      return str;
    }
    //根据教练名称模糊搜索教练信息
    string CoachMessageSearch(ManageDB*& md_,string name)
    {
      // 将用户的信息存储在链表中                     
      list<CoachMessage> lcm;                     
      lcm.clear();                     
      //统计用户信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define CoachMessageSearch_SQL "select * from CoachInfo where coach_name like \'%%%s%%\';"
      snprintf(sql,sizeof(sql)-1,CoachMessageSearch_SQL,name.c_str());
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : CoachMessageSearch, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int coach_id = atoi(row[0]);                                                                                          
        string coach_name(row[1]);
        string coach_phone(row[2]);
        int coach_sex = atoi(row[3]);                   
        int coach_age = atoi(row[4]);                     
        string coach_date(row[5]);
        int coach_educateAge = atoi(row[6]);                  
        double coach_salary = stod(string(row[7]));
        int coach_state = atoi(row[8]);

        //将登录时间初始化为Data格式
        Date date(coach_date);
        //加上yay-mm-dd 中的0 ，比如2021-2-1 --> 2021-02-01,目的在于统一时间的显示
        coach_date = date.ToString();


        CoachMessage cm(coach_id,coach_name,coach_phone,coach_sex,coach_age,coach_date,coach_educateAge,coach_salary,coach_state);
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
    bool AddCoachMessage(ManageDB*& md_,Json::Value& v)
    {
      string coach_name = v["coach_name"].asString();                     
      string coach_phone = v["coach_phone"].asString();                    
      int coach_sex = stoi(v["coach_sex"].asString());                         
      int coach_age = stoi(v["coach_age"].asString());                                                   
      string coach_date = v["coach_date"].asString();                                                               
      int coach_educateAge = stoi(v["coach_educateAge"].asString());
      double coach_salary = stod(v["coach_salary"].asString());


      char sql[1024]={0};     
#define AddCoachMessage_SQL "insert into CoachInfo values(null,\'%s\',\'%s\',%d,%d,\'%s\',%d,%lf,0);"
      snprintf(sql,sizeof(sql)-1,AddCoachMessage_SQL,coach_name.c_str(),coach_phone.c_str(),coach_sex,coach_age,coach_date.c_str(),coach_educateAge,coach_salary);


      return md_->ExecSQL(sql); 
    }

    // 根据教练id查询教练信息
    string IdToCoachMessage(ManageDB*& md_,int tid)
    {
      char sql[1024]={0};
#define IdToCoachMessage_SQL "select * from CoachInfo where coach_id = %d;"
      snprintf(sql,sizeof(sql)-1,IdToCoachMessage_SQL,tid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberCoachQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }

      MYSQL_ROW row = mysql_fetch_row(res);
      int coach_id = atoi(row[0]);                                                                                          
      string coach_name(row[1]);
      string coach_phone(row[2]);
      int coach_sex = atoi(row[3]);                   
      int coach_age = atoi(row[4]);                     
      string coach_date(row[5]);
      int coach_educateAge = atoi(row[6]);                  
      double coach_salary = stod(string(row[7]));
      int coach_state = atoi(row[8]);

      CoachMessage cm(coach_id,coach_name,coach_phone,coach_sex,coach_age,coach_date,coach_educateAge,coach_salary,coach_state);

      mysql_free_result(res);

      return xpack::json::encode(cm);
    }
    //修改教练信息
    bool UpdateCoachMessage(ManageDB*& md_,Json::Value& v)
    {
      int coach_id = stoi(v["coach_id"].asString());
      string coach_name = v["coach_name"].asString();                     
      string coach_phone = v["coach_phone"].asString();                    
      int coach_sex = stoi(v["coach_sex"].asString());                         
      int coach_age = stoi(v["coach_age"].asString());                                                   
      string coach_date = v["coach_date"].asString();                                                               
      int coach_educateAge = stoi(v["coach_educateAge"].asString());
      double coach_salary = stod(v["coach_salary"].asString());
      int coach_state = stoi(v["coach_state"].asString());

      char sql[1024]={0};
#define UpdateCoachMessage_SQL "update CoachInfo set coach_name =\'%s\', coach_phone=\'%s\',coach_sex=%d,coach_age=%d,coach_date=\'%s\',coach_educateAge=%d,coach_salary=%lf,coach_state=%d  where coach_id=%d;"
      snprintf(sql,sizeof(sql)-1,UpdateCoachMessage_SQL,coach_name.c_str(),coach_phone.c_str(),coach_sex,coach_age,coach_date.c_str(),coach_educateAge,coach_salary,coach_state,coach_id);
      return md_->ExecSQL(sql);
    }
    //删除教练信息
    bool DelCoachMessage(ManageDB*& md_,Json::Value& v)
    {
      int coach_id = stoi(v["coach_id"].asString());
      char sql[1024]={0};
#define DelCoachMessage_SQL "delete from CoachInfo where coach_id=%d;"
      snprintf(sql,sizeof(sql)-1,DelCoachMessage_SQL,coach_id);
      return md_->ExecSQL(sql);
    }


};


