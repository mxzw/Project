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

//这是显示会员卡信息的结构体
struct CardMessage
{
  CardMessage(int tid=0,string tname="",int tday=0,int tmoney=0):type_id(tid),type_name(tname),type_day(tday),type_money(tmoney)
  {}
  CardMessage(const CardMessage& cm)
  {
    type_id = cm.type_id;
    type_name = cm.type_name;
    type_day = cm.type_day;
    type_money = cm.type_money;
  }
  int type_id;
  string type_name;
  int type_day;
  int type_money;
  XPACK(O(type_id,type_name,type_day,type_money));
};

//组织成json格式发送给浏览器
//这是显示会员信息的结构体
struct UserMessage    
{    

  UserMessage(int mid,string mname,string mphone,int msex,int mage,struct CardMessage ctype,string ldate,int mstate,string edate)    
    :member_id(mid),member_name(mname),member_phone(mphone),member_sex(msex), member_age(mage),    
    card_type(ctype),login_date(ldate),member_state(mstate),expire_date(edate)    
  {}                                                                                                  
  UserMessage(){}
  UserMessage(const UserMessage& um)
  {
    member_id = um.member_id;
    member_name = um.member_name;
    member_phone = um.member_phone;
    member_sex = um.member_sex;
    member_age = um.member_age;
    card_type = um.card_type;
    login_date = um.login_date;
    member_state = um.member_state;
    expire_date = um.expire_date;
  }

  int member_id;                                                                                      
  string member_name;                 
  string member_phone;    
  int member_sex;        
  int member_age;     
  struct CardMessage card_type;
  string login_date;    
  int member_state;    
  string expire_date;    
  XPACK(O(member_id,member_name,member_phone,member_sex,member_age,card_type,login_date,member_state,expire_date));    
};  


class Member
{
  public:
    Member(){}
    ~Member(){}
    //删除一个会员
    bool DelMember(ManageDB*& md_,Json::Value& v)
    {
      int member_id = stoi(v["member_id"].asString());
      char sql[1024]={0};
#define DelMember_SQL "delete from MemberInfo where member_id=%d;"
      snprintf(sql,sizeof(sql)-1,DelMember_SQL,member_id);

      return md_->ExecSQL(sql);

    }
    // 添加一个会员
    bool AddMember(ManageDB*& md_,Json::Value& v)
    {
      string member_name = v["member_name"].asString();
      string member_phone = v["member_phone"].asString();
      int member_sex = stoi(v["member_sex"].asString());
      int member_typeid = stoi(v["member_typeid"].asString());
      int member_age = stoi(v["member_age"].asString());
      string login_date = v["login_date"].asString();

      char sql[1024]={0};
#define AddMember_SQL "insert into MemberInfo values(null,\'%s\',\'%s\',%d,%d,%d,1,\'%s\',0,0);"
      snprintf(sql,sizeof(sql)-1,AddMember_SQL,member_name.c_str(),member_phone.c_str(),member_sex,member_age,member_typeid,login_date.c_str());

      return md_->ExecSQL(sql);

    }
    //查询所有会员卡的类型
    string MemberCardQuery(ManageDB*& md_)
    {
      list<CardMessage> lcm;
      lcm.clear();
      char sql[1024]={0};
#define CardQuery_SQL "select * from MemberCardType;"
      snprintf(sql,sizeof(sql)-1,CardQuery_SQL);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberCardQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }

      MYSQL_ROW row;
      while((row = mysql_fetch_row(res))!=NULL)
      {
        int type_id = atoi(row[0]);
        string type_name = row[1];
        int type_day = atoi(row[2]);
        int type_money = atoi(row[3]);

        CardMessage cm(type_id,type_name,type_day,type_money);
        lcm.push_back(cm);
      }

      mysql_free_result(res);

      return xpack::json::encode(lcm);
    }
    //搜索相关会员卡信息
    string CardMessageSearch(ManageDB*& md_,string type_name)
    {
      list<CardMessage> lcm;
      lcm.clear();
      char sql[1024]={0};
#define CardMessageSearch_SQL "select * from MemberCardType where type_name like \'%s%%\';"
      snprintf(sql,sizeof(sql)-1,CardMessageSearch_SQL,type_name.c_str());
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberCardQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }

      MYSQL_ROW row;
      while((row = mysql_fetch_row(res))!=NULL)
      {
        int type_id = atoi(row[0]);
        string type_name = row[1];
        int type_day = atoi(row[2]);
        int type_money = atoi(row[3]);

        CardMessage cm(type_id,type_name,type_day,type_money);
        lcm.push_back(cm);
      }

      mysql_free_result(res);

      return xpack::json::encode(lcm);
    }
    // 修改会员卡表中的信息
    bool UpdateCardMessage(ManageDB*& md_,Json::Value& v)
    {
      int type_id = stoi(v["type_id"].asString());
      string type_name = v["type_name"].asString();
      int type_day = stoi(v["type_day"].asString());
      int type_money = stoi(v["type_money"].asString());
      char sql[1024]={0};
#define UpdateCardMessage_SQL "update MemberCardType set type_name =\'%s\',type_day=%d,type_money=%d where type_id=%d;"
      snprintf(sql,sizeof(sql)-1,UpdateCardMessage_SQL,type_name.c_str(),type_day,type_money,type_id);
      return md_->ExecSQL(sql);
    }
    //新增一个会员卡
    bool AddCardMessage(ManageDB*& md_,Json::Value& v)
    {
      string type_name = v["type_name"].asString();
      int type_day = stoi(v["type_day"].asString());
      int type_money = stoi(v["type_money"].asString());
      char sql[1024]={0};
#define AddCardMessage_SQL "insert into MemberCardType values(null,\'%s\',%d,%d);"
      snprintf(sql,sizeof(sql)-1,AddCardMessage_SQL,type_name.c_str(),type_day,type_money);
      return md_->ExecSQL(sql);
    }
    //删除一个会员卡
    bool DelCardMessage(ManageDB*& md_,Json::Value& v)
    {
      int type_id = stoi(v["type_id"].asString());
      char sql[1024]={0};
#define DelCardMessage_SQL "delete from MemberCardType where type_id=%d;"
      snprintf(sql,sizeof(sql)-1,DelCardMessage_SQL,type_id);
      return md_->ExecSQL(sql);

    }


    //搜素相关用户信息
    string MemberMessageSearch(ManageDB*& md_,Json::Value& v)
    {
      // 将用户的信息存储在链表中
      list<UserMessage> lum;
      lum.clear();
      //统计用户信息的个数
      int countNum = 0;

      string search_name = v["search_name"].asString();
      int search_typeid = stoi(v["search_typeid"].asString());

      char sql[1024]={0};
#define MemberSearch_SQL "select member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state,increase_day from MemberInfo where member_name like \'%%%s%%\' or member_typeid = %d;"
      snprintf(sql,sizeof(sql)-1,MemberSearch_SQL,search_name.c_str(),search_typeid);
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
        // member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state 
        int member_id = atoi(row[0]);
        string member_name(row[1]);
        string member_phone(row[2]);
        int member_sex = atoi(row[3]);
        int member_age = atoi(row[4]);
        int member_typeid = atoi(row[5]);
        string login_date(row[6]);
        int member_state = atoi(row[7]);
        int increase_day = atoi(row[8]);

        int cardId = member_typeid;
        //查询该用户所持有会员卡的所有信息
        struct CardMessage card_type;
        QueryAllCardMessage(md_,cardId,card_type);
        //组织查询会员卡的类型的时间，用于计算到期时间
        int cardTime = card_type.type_day;

        //将登录时间初始化为Data格式
        string tmp(login_date);
        Date date(tmp);
        //加上yay-mm-dd 中的0 ，比如2021-2-1 --> 2021-02-01,目的在于统一时间的显示
        login_date = date.ToString();

        date += cardTime + increase_day;

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
        UserMessage um(member_id,member_name,member_phone,member_sex,member_age,card_type,login_date,member_state,expire_date);
        lum.push_back(um);
        //统计数据的个数
        countNum++;
      }
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(lum);

      string prev = "{\"count\":"+ to_string(countNum) +",\"data\":";
      str = prev + str + "}";  
      return str;

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
#define MemberQuery_SQL "select member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state,increase_day from MemberInfo;"
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
        int increase_day = atoi(row[8]);

        int cardId = member_typeid;
        //查询该用户所持有会员卡的所有信息
        struct CardMessage card_type;
        QueryAllCardMessage(md_,cardId,card_type);
        //组织查询会员卡的类型的时间，用于计算到期时间
        int cardTime = card_type.type_day;

        //将登录时间初始化为Data格式
        string tmp(login_date);
        Date date(tmp);
        //加上yay-mm-dd 中的0 ，比如2021-2-1 --> 2021-02-01,目的在于统一时间的显示
        login_date = date.ToString();

        date += cardTime + increase_day;

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
        UserMessage um(member_id,member_name,member_phone,member_sex,member_age,card_type,login_date,member_state,expire_date);
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
    //根据id查询所有的会员信息
    void QueryAllMemberMessage(ManageDB*& md_,int mid,struct UserMessage& um)
    {
      char sql[1024]={0};
#define QueryAllMemberMessage_SQL "select member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state,increase_day from MemberInfo where member_id = %d;"
      snprintf(sql,sizeof(sql)-1,QueryAllMemberMessage_SQL,mid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return;
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      // member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state 
      string member_name(row[0]);
      string member_phone(row[1]);
      int member_sex = atoi(row[2]);
      int member_age = atoi(row[3]);
      int member_typeid = atoi(row[4]);
      string login_date(row[5]);
      int member_state = atoi(row[6]);
      int increase_day = atoi(row[7]);

      int cardId = member_typeid;
      //查询该用户所持有会员卡的所有信息
      struct CardMessage card_type;
      QueryAllCardMessage(md_,cardId,card_type);
      //组织查询会员卡的类型的时间，用于计算到期时间
      int cardTime = card_type.type_day;

      //将登录时间初始化为Data格式
      string tmp(login_date);
      Date date(tmp);
      //加上yay-mm-dd 中的0 ，比如2021-2-1 --> 2021-02-01,目的在于统一时间的显示
      login_date = date.ToString();

      date += cardTime + increase_day;

      //用于存储过期时间
      string expire_date = date.ToString();


      //若过期时间 < 当前时间，说明当前会员卡已经过期，需要续卡
      if(expire_date < Date::GetNowDate())
      {
        //修改Member表中的member_state
        member_state = -1;
        if(!UpdateMenberState(md_,member_state,mid))
        {
          cout << "UpdateMenberState faild " << endl;
          mysql_free_result(res);
          return ;
        }
      }
      //用来存储从数据库中读出的用户的信息
      um.member_id = mid;
      um.member_name = member_name;
      um.member_phone = member_phone;
      um.member_sex = member_sex;
      um.member_age = member_age;
      um.card_type = card_type;
      um.login_date = login_date;
      um.member_state = member_state;
      um.expire_date = expire_date;
      //统计数据的个数
      mysql_free_result(res);
    }
    //根据id查询所有的会员信息
    string IdToMemberMessage(ManageDB*& md_,int mid)
    {
      char sql[1024]={0};
#define QueryAllMemberMessage_SQL "select member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state,increase_day from MemberInfo where member_id = %d;"
      snprintf(sql,sizeof(sql)-1,QueryAllMemberMessage_SQL,mid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      // member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state 
      string member_name(row[0]);
      string member_phone(row[1]);
      int member_sex = atoi(row[2]);
      int member_age = atoi(row[3]);
      int member_typeid = atoi(row[4]);
      string login_date(row[5]);
      int member_state = atoi(row[6]);
      int increase_day = atoi(row[7]);

      int cardId = member_typeid;
      //查询该用户所持有会员卡的所有信息
      struct CardMessage card_type;
      QueryAllCardMessage(md_,cardId,card_type);
      //组织查询会员卡的类型的时间，用于计算到期时间
      int cardTime = card_type.type_day;

      //将登录时间初始化为Data格式
      string tmp(login_date);
      Date date(tmp);
      //加上yay-mm-dd 中的0 ，比如2021-2-1 --> 2021-02-01,目的在于统一时间的显示
      login_date = date.ToString();

      date += cardTime + increase_day;

      //用于存储过期时间
      string expire_date = date.ToString();


      //若过期时间 < 当前时间，说明当前会员卡已经过期，需要续卡
      if(expire_date < Date::GetNowDate())
      {
        //修改Member表中的member_state
        member_state = -1;
        if(!UpdateMenberState(md_,member_state,mid))
        {
          cout << "UpdateMenberState faild " << endl;
          mysql_free_result(res);
          return "";
        }
      }
      //用来存储从数据库中读出的用户的信息
      UserMessage um(mid,member_name,member_phone,member_sex,member_age,card_type,login_date,member_state,expire_date);
      //统计数据的个数
      mysql_free_result(res);
      return xpack::json::encode(um);
    }

    void QueryAllCardMessage(ManageDB*& md_,int mid,struct CardMessage& cm)
    {
      char sql[1024]={0};
#define QueryAllCardMessage_SQL "select * from MemberCardType where type_id = %d;"
      snprintf(sql,sizeof(sql)-1,QueryAllCardMessage_SQL,mid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : QueryAllCardMessage, sql is " << sql << endl;
        mysql_free_result(res);
        return ;
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      cm.type_id = atoi(row[0]);
      cm.type_name = row[1];
      cm.type_day = atoi(row[2]);
      cm.type_money = atoi(row[3]);
      mysql_free_result(res);
    }
    //修改会员表中的用户状态
    bool UpdateMenberState(ManageDB*& md_,int mstate,int mid)
    {
      char sql[1024]={0};
#define UpdateMenberState_SQL "update MemberInfo set member_state = %d where member_id = %d;"
      snprintf(sql,sizeof(sql)-1,UpdateMenberState_SQL,mstate,mid);

      return md_->ExecSQL(sql);
    }
    bool UpdateMemberMessage(ManageDB*& md_,Json::Value& v)
    {
      string member_name = v["member_name"].asString();
      string member_phone = v["member_phone"].asString();
      int member_id = stoi(v["member_id"].asString());
      int member_sex = stoi(v["member_sex"].asString());
      int member_typeid = stoi(v["member_typeid"].asString());
      int member_age = stoi(v["member_age"].asString());
      string login_date = v["login_date"].asString();

      char sql[1024]={0};
#define UpdateMemberMessage_SQL "update MemberInfo set member_name = \'%s\',member_phone = \'%s\',member_sex = %d,member_age = %d,member_typeid = %d,login_date = \'%s\' where member_id = %d;"
      snprintf(sql,sizeof(sql)-1,UpdateMemberMessage_SQL,member_name.c_str(),member_phone.c_str(),member_sex,member_age,member_typeid,login_date.c_str(),member_id);
      cout << "UpdateMemberMessage sql is : " << sql << endl;

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
    //根据会员卡id查询会员卡对应的具体金额
    string QueryCardMoney(ManageDB*& md_,Json::Value& v)
    {
      int type_id = stoi(v["type_id"].asString());
      char sql[1024]={0};
#define QueryCardMoney_SQL "select type_money from MemberCardType where type_id = %d;"
      snprintf(sql,sizeof(sql)-1,QueryCardMoney_SQL,type_id);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : QueryCardMoney, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);
      return row[0];
    }
    //根据member_id 获取对应的会员卡类型天数
    int GetCardDay(ManageDB*& md_,int mid)
    {
      char sql[1024]={0};
#define GetCardDay_SQL "select type_day from MemberCardType where type_id = (select member_typeid from MemberInfo where member_id = %d);"
      snprintf(sql,sizeof(sql)-1,GetCardDay_SQL,mid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetCardDay, sql is " << sql << endl;
        mysql_free_result(res);
        return 0;
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);
      return atoi(row[0]);
    }
    //根据member_id获取对应的会员卡类型的天数，并将其加到increase_day这一列中
    bool UpdateMemberIncreaseDay(ManageDB*& md_,int mid)
    {
      int type_day = GetCardDay(md_,mid);
      char sql[1024]={0};
#define UpdateMemberIncreaseDay_SQL "update MemberInfo set increase_day = increase_day + %d where member_id = %d;"
      snprintf(sql,sizeof(sql)-1,UpdateMemberIncreaseDay_SQL,type_day,mid);
      return md_->ExecSQL(sql);
    }
    //根据type_id 修改对应用户的会员卡类型
    bool UpdateMemberCardType(ManageDB*& md_,int mid,int tid)
    {
      char sql[1024]={0};
#define UpdateMemberCardType_SQL "update MemberInfo set member_typeid = %d where member_id = %d;"
      snprintf(sql,sizeof(sql)-1,UpdateMemberCardType_SQL,tid,mid);
      return md_->ExecSQL(sql);
    }
    string IdToCardMessage(ManageDB*& md_,int tid)
    {
      char sql[1024]={0};
#define IdToCardMessage_SQL "select * from MemberCardType where type_id = %d;"
      snprintf(sql,sizeof(sql)-1,IdToCardMessage_SQL,tid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberCardQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }

      MYSQL_ROW row = mysql_fetch_row(res);
      int type_id = atoi(row[0]);
      string type_name = row[1];
      int type_day = atoi(row[2]);
      int type_money = atoi(row[3]);

      CardMessage cm(type_id,type_name,type_day,type_money);
      mysql_free_result(res);

      return xpack::json::encode(cm);
    }

};
