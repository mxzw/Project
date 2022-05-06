#include <unistd.h>

#include <jsoncpp/json/json.h>

#include <iostream>
#include <string>
#include <list>

#include "ManageDB.hpp"
#include "./xpack-master/json.h"


using namespace std;

struct EquipmentMessage
{
  EquipmentMessage(int cid=0,string cname="",string ctext=0,int cnum=0)
    :eq_id(cid),eq_name(cname),eq_text(ctext),eq_num(cnum)
  {}
  int eq_id;
  string eq_name;
  string eq_text;
  int eq_num;
  XPACK(O(eq_id,eq_name,eq_text,eq_num,eq_num));
};

class Equipment{
  public:
    Equipment(){}
    ~Equipment(){}

    //查询所有的器材信息
    string EquipmentMessageQuery(ManageDB*& md_)
    {
      // 将器材的信息存储在链表中                     
      list<EquipmentMessage> lcm;                     
      lcm.clear();                     
      //统计器材信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define EquipmentMessageQuery_SQL "select * from EquipmentInfo;"
      snprintf(sql,sizeof(sql)-1,EquipmentMessageQuery_SQL);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : EquipmentMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int eq_id = atoi(row[0]);                                                                                          
        string eq_name(row[1]);
        string eq_text = string(row[2]);
        int eq_num = atoi(row[3]);

        EquipmentMessage cm(eq_id,eq_name,eq_text,eq_num);
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
    //查询所有的器材信息
    string EquipmentMessageQuery(ManageDB*& md_,int mid)
    {
      // 将器材的信息存储在链表中                     
      list<EquipmentMessage> lcm;                     
      lcm.clear();                     
      //统计器材信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define EquipmentMessageQuery2_SQL "select * from EquipmentInfo where eq_id = any( select eq_id from MemberEquip where member_id=%d);"
      snprintf(sql,sizeof(sql)-1,EquipmentMessageQuery2_SQL,mid);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : EquipmentMessageQuery2, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int eq_id = atoi(row[0]);                                                                                          
        string eq_name(row[1]);
        string eq_text = string(row[2]);
        int eq_num = atoi(row[3]);

        EquipmentMessage cm(eq_id,eq_name,eq_text,eq_num);
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

    //根据器材名来模糊搜索器材信息
    string EquipmentMessageSearch(ManageDB*& md_,string eq_name)
    {
      // 将器材的信息存储在链表中                     
      list<EquipmentMessage> lcm;                     
      lcm.clear();                     
      //统计器材信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define EquipmentMessageSearch_SQL "select * from EquipmentInfo where eq_name like \'%%%s%%\';"
      snprintf(sql,sizeof(sql)-1,EquipmentMessageSearch_SQL,eq_name.c_str());
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : EquipmentMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int eq_id = atoi(row[0]);                                                                                          
        string eq_name(row[1]);
        string eq_text = string(row[2]);
        int eq_num = atoi(row[3]);

        EquipmentMessage cm(eq_id,eq_name,eq_text,eq_num);
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

    //新增器材信息
    bool AddEquipmentMessage(ManageDB*& md_,Json::Value& v)    
    {    
      string eq_name = v["eq_name"].asString();                         
      string eq_text = v["eq_text"].asString();    
    
      char sql[1024]={0};         
#define AddEquipmentMessage_SQL "insert into EquipmentInfo values(null,\'%s\',\'%s\');"
      snprintf(sql,sizeof(sql)-1,AddEquipmentMessage_SQL,eq_name.c_str(),eq_text.c_str());    
    
      return md_->ExecSQL(sql);     
    } 
    // 根据器材id查询器材信息
    string IdToEquipmentMessage(ManageDB*& md_,int tid)
    {
      char sql[1024]={0};
#define IdToEquipmentMessage_SQL "select * from EquipmentInfo where eq_id = %d;"
      snprintf(sql,sizeof(sql)-1,IdToEquipmentMessage_SQL,tid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : IdToEquipmentMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      int eq_id = atoi(row[0]);                                                                                          
      string eq_name(row[1]);
      string eq_text(row[2]);
      int eq_num = atoi(row[3]);

      EquipmentMessage cm(eq_id,eq_name,eq_text,eq_num);
      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(cm);
      return str;
    }
    //修改教练信息
    bool UpdateEquipmentMessage(ManageDB*& md_,Json::Value& v)
    {
      int eq_id = stoi(v["eq_id"].asString());
      string eq_name = v["eq_name"].asString();                         
      string eq_text = v["eq_text"].asString();
      int eq_num = stoi(v["eq_num"].asString());

      char sql[1024]={0};
#define UpdateEquipmentMessage_SQL "update EquipmentInfo set eq_name =\'%s\',eq_text=\'%s\',eq_num=%d where eq_id=%d;"
      snprintf(sql,sizeof(sql)-1,UpdateEquipmentMessage_SQL,eq_name.c_str(),eq_text.c_str(),eq_num,eq_id);
      return md_->ExecSQL(sql);
    }

    //删除教练信息
    bool DelEquipmentMessage(ManageDB*& md_,Json::Value& v)
    {
      int eq_id = stoi(v["eq_id"].asString());
      char sql[1024]={0};
#define DelEquipmentMessage_SQL "delete from EquipmentInfo where eq_id=%d;"
      snprintf(sql,sizeof(sql)-1,DelEquipmentMessage_SQL,eq_id);
      return md_->ExecSQL(sql);
    }

};
