#pragma once                                                                                                                                                            
#include <iostream>          
#include <jsoncpp/json/json.h>    
#include <string>      
#include <list>            

#include "ManageDB.hpp"    
#include "Date.hpp"    
#include "CoachCourse.hpp"

class User
{
  public:
    User(){}
    ~User(){}

    string GetUserEmail(ManageDB*& md_,int user_id)
    {
      char sql[1024]={0};
#define GetUserEmail_SQL "select user_email from LoginUserInfo where user_id=%d"
      snprintf(sql,sizeof(sql)-1,GetUserEmail_SQL,user_id);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetUserEmail, sql is " << sql << endl;    
        mysql_free_result(res);    
        return "";  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      return row[0];
    }

    void GetUserMessage(ManageDB*& md_,Json::Value& v,int user_id)
    {
      char sql[1024]={0};
#define GetUserMessage_SQL "select * from MemberInfo where member_userid=%d;"
      snprintf(sql,sizeof(sql)-1,GetUserMessage_SQL,user_id);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetUserEmail, sql is " << sql << endl;    
        mysql_free_result(res);    
        return ;  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      v["email"] = GetUserEmail(md_,user_id);
      if(row == NULL)
      {
        v["status"] = 0;
        return ;
      }


      int mid = atoi(row[0]);
      int tid = atoi(row[5]);
      v["member_name"] = row[1];
      v["member_sex"] = row[3];
      v["member_age"] = row[4];
      v["member_phone"] = row[2];
      v["card_name"] = GetCardName(md_,tid);
      v["coach_name"] = GetCoachName(md_,mid);
      v["course_name"] = GetCourseName(md_,mid);
      v["equipment_name"] = GetEquipName(md_,mid);

    }
    string GetCardName(ManageDB*& md_,int tid)
    {
      char sql[1024]={0};
#define GetCardName_SQL "select type_name from MemberCardType where type_id=%d;"
      snprintf(sql,sizeof(sql)-1,GetCardName_SQL,tid);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetCardName, sql is " << sql << endl;    
        mysql_free_result(res);    
        return "";  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      return row == NULL ? "" : row[0];
    }
    string GetCoachName(ManageDB*& md_,int mid)
    {
      char sql[1024]={0};
#define GetCoachName_SQL "select coach_name from CoachInfo where coach_id=(select coach_id from CoachCourseList where member_id=%d limit 1);"
      snprintf(sql,sizeof(sql)-1,GetCoachName_SQL,mid);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetCoachName, sql is " << sql << endl;    
        mysql_free_result(res);    
        return "";  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      return row == NULL ? "" : row[0];
    }
    string GetCourseName(ManageDB*& md_,int mid)
    {
      char sql[1024]={0};
#define GetCourseName_SQL "select course_name from CourseInfo where course_id=(select course_id from CoachCourseList where member_id=%d limit 1);"
      snprintf(sql,sizeof(sql)-1,GetCourseName_SQL,mid);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetCourseName, sql is " << sql << endl;    
        mysql_free_result(res);    
        return "";  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      return row == NULL ? "" : row[0];
    }
    string GetEquipName(ManageDB*& md_,int mid)
    {
      char sql[1024]={0};
#define GetEquipName_SQL "select eq_name from EquipmentInfo where eq_id=(select eq_id from MemberEquip where member_id=%d limit 1);"
      snprintf(sql,sizeof(sql)-1,GetEquipName_SQL,mid);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetEquipName, sql is " << sql << endl;    
        mysql_free_result(res);    
        return "";  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      return row == NULL ? "" : row[0];
    }

    bool UpdateUserMessage(ManageDB*& md_, Json::Value& v)
    {
      int uid = stoi(v["user_id"].asString());
      string name = v["name"].asString();
      int age = stoi(v["age"].asString());
      int sex = stoi(v["sex"].asString());
      string phone = v["phone"].asString();
      string email = v["email"].asString();
      char sql1[1024] = {0};
#define UpdateUser1_SQL "update MemberInfo set member_name = \"%s\",member_age=%d,member_sex=%d,member_phone=\"%s\" where member_userid=%d"
      snprintf(sql1,sizeof(sql1)-1,UpdateUser1_SQL,name.c_str(),age,sex,phone.c_str(),uid);

      char sql2[1024] = {0};
#define UpdateUser2_SQL "update LoginUserInfo set user_email = \"%s\" where user_id=%d"
      snprintf(sql2,sizeof(sql1)-1,UpdateUser2_SQL,email.c_str(),uid);

      return md_->ExecSQL(sql1) && md_->ExecSQL(sql2);
    }

    int JudgeUserStatus(ManageDB*& md_,int user_id)
    {
      char sql[1024]={0};
#define JudgeUserStatus_SQL "select member_id from MemberInfo where member_userid=%d;"
      snprintf(sql,sizeof(sql)-1,JudgeUserStatus_SQL,user_id);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : JudgeUserStatus, sql is " << sql << endl;    
        mysql_free_result(res);    
        return -1;  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      return row == NULL ? -1 : atoi(row[0]);
    }
    int OrderEquipment(ManageDB*& md_,int eid,int mid)    
    {    
      char sql1[1024]={0};    
#define OrderEquipment_SQL "update EquipmentInfo set eq_num = eq_num - 1 where eq_id=%d;"    
      snprintf(sql1,sizeof(sql1)-1,OrderEquipment_SQL,eid);    

      //如果该用户过该器材
      if(GetMemberEquipMessage(md_,mid,eid))
      {
        cout << "该器材已被预约。。。。" << endl;
        return 2; //代表该器材已经被预约了
      }
      char sql2[1024]={0};    
#define AddMemberEquip_SQL "insert into MemberEquip values(null,%d,%d);"    
      snprintf(sql2,sizeof(sql2)-1,AddMemberEquip_SQL,eid,mid);
      return (md_->ExecSQL(sql1) && md_->ExecSQL(sql2)) ? 1 : 0;    
    }    
    //获取用户预约器材信息
    bool GetMemberEquipMessage(ManageDB*& md_,int mid,int eid)
    {
      char sql[1024] = {0};
#define GetMemberEquipMessage_SQL "select * from MemberEquip where member_id=%d and eq_id = %d;"
      snprintf(sql,sizeof(sql)-1,GetMemberEquipMessage_SQL,mid,eid);

      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : GetMemberEquipMessage, sql is " << sql << endl;    
        mysql_free_result(res);    
        return true;  
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      mysql_free_result(res);    
      return row == NULL ? false : true;
    }


    bool UnOrderEquipment(ManageDB*& md_,int eid,int mid)     
    {                                                                                    
      char sql[1024]={0};                                                                
#define UnOrderEquipment_SQL "update EquipmentInfo set eq_num = eq_num + 1 where eq_id=%d;"    
      snprintf(sql,sizeof(sql)-1,UnOrderEquipment_SQL,eid);    
      return md_->ExecSQL(sql) && DelMemberEquip(md_,mid,eid);   
    } 
    bool DelMemberEquip(ManageDB*& md_,int mid,int eid)
    {
      char sql[1024] = {0};
#define DelMemberEquip_SQL "delete from MemberEquip where member_id=%d and eq_id=%d;"
      snprintf(sql,sizeof(sql)-1,DelMemberEquip_SQL,mid,eid);    
      return md_->ExecSQL(sql);   
    }
};
