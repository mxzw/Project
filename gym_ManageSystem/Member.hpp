#pragma once
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>
#include <list>
#include "ManageDB.hpp"
#include "Date.hpp"

//实现结构体转json功能的库
#include "./xpack-master/json.h"
// 首先需要在结构体中对成员变量进行注册，然后使用 xpack::json::encode() 函数即可获得对应的json数据

using namespace std;

//组织成json格式发送给浏览器
struct UserMessage    
{    

  UserMessage(int mid,string mname,string mphone,int msex,int mage,string cname,string ldate,int mstate,string edate)    
    :member_id(mid),member_name(mname),member_phone(mphone),member_sex(msex), member_age(mage),    
    card_name(cname),login_date(ldate),member_state(mstate),expire_date(edate)    
  {}                                                                                                  

  int member_id;                                                                                      
  string member_name;                 
  string member_phone;    
  int member_sex;        
  int member_age;     
  string card_name;
  string login_date;    
  int member_state;    
  string expire_date;    
  XPACK(O(member_id,member_name,member_phone,member_sex,member_age,card_name,login_date,member_state,expire_date));    
};  

class Member
{
  public:
    Member(){}
    ~Member(){}
    //查询会员卡的类型
    void MemberCardQuery(ManageDB*& md_,Json::Value& v)
    {
      char sql[1024]={0};
#define CardQuery_SQL "select * from MemberCardType;"
      snprintf(sql,sizeof(sql)-1,CardQuery_SQL);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberCardQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return ;
      }

      MYSQL_ROW row = mysql_fetch_row(res);

      v["typeId"] = row[0];
      v["typeName"] = row[1];
      v["typeDay"] = row[2];
      v["typeMoney"] = row[3];

      mysql_free_result(res);
    }
    //查询用户信息 --->返回组织好的json数据
    string MemberMessageQuery(ManageDB*& md_)
    {
      // 将用户的信息存储在链表中
      list<UserMessage> lum;
      lum.clear();
      //统计用户信息的个数
      int countNum = 0;

      char sql[1024]={0};
#define MemberQuery_SQL "select member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state from MemberInfo;"
      snprintf(sql,sizeof(sql)-1,MemberQuery_SQL);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中 
      MYSQL_ROW row;
      while((row = mysql_fetch_row(res))!=NULL)
      {
        /*
           for(unsigned int i = 0;i < len; ++i)
           {
           cout << row[i] << " ";

           }
           cout << endl;*/
        // member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state 
        int member_id = atoi(row[0]);
        string member_name(row[1]);
        string member_phone(row[2]);
        int member_sex = atoi(row[3]);
        int member_age = atoi(row[4]);
        int member_typeid = atoi(row[5]);
        string login_date(row[6]);
        int member_state = atoi(row[7]);

        int cardId = member_typeid;
        //组织查询会员卡的类型的时间，用于计算到期时间
        int cardTime = QueryCardTime(md_,cardId);
        //查询会员卡类型
        string cardName = QueryCardName(md_,cardId);

        //将登录时间初始化为Data格式
        string tmp(login_date);
        Date date(tmp);
        //去除yy-mm-dd 中的0 ，比如2021-02-01 --> 2021-2-1,目的在于统一时间的显示
        login_date = date.ToString();

        date += cardTime;

        //用于存储过期时间
        string expire_date = date.ToString();


        //若过期时间 < 当前时间，说明当前会员卡已经过期，需要续卡
        if(expire_date < Date::GetNowDate())
        {
          //修改Member表中的member_state
          member_state = -1;
          if(!UpdateMenberState(md_,member_state,member_id))
          {
            cout << "UpdateMenberState faild " << endl;
            mysql_free_result(res);
            return "";
          }
        }
        //用来存储从数据库中读出的用户的信息
        UserMessage um(member_id,member_name,member_phone,member_sex,member_age,cardName,login_date,member_state,expire_date);
        lum.push_back(um);
        //统计数据的个数
        countNum++;
      }
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(lum);

      //开始组织json数据，即发送给bootstrapTable使用的json数据    
      //client模式：{"count":xxx,"data":[{},{},{}]}                                                                                                                   
      //server模式：{"total":xxx,"rows":[{},{},{}]}
      string prev = "{\"count\":"+ to_string(countNum) +",\"data\":";
      str = prev + str + "}";  
      return str;

    }
    //修改会员表中的用户状态
    bool UpdateMenberState(ManageDB*& md_,int mstate,int mid)
    {
      char sql[1024]={0};
#define UpdateMenberState_SQL "update MemberInfo set member_state = %d where member_id = %d;"
      snprintf(sql,sizeof(sql)-1,UpdateMenberState_SQL,mstate,mid);

      return md_->ExecSQL(sql);
    }

    //查询会员卡类型的日期
    int QueryCardTime(ManageDB*& md_,int m_typeid)
    {
      char sql[1024]={0};
#define QueryCardTime_SQL "select type_day from MemberCardType where type_id = %d;"
      snprintf(sql,sizeof(sql)-1,QueryCardTime_SQL,m_typeid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : QueryCardTime, sql is " << sql << endl;
        mysql_free_result(res);
        return 0;
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);
      return atoi(row[0]);
    }

    //根据会员卡id查询会员卡对应具体名称
    string QueryCardName(ManageDB*& md_,int m_typeid)
    {
      char sql[1024]={0};
#define QueryCardName_SQL "select type_name from MemberCardType where type_id = %d;"
      snprintf(sql,sizeof(sql)-1,QueryCardName_SQL,m_typeid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : QueryCardTime, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);
      return row[0];
    }

};
