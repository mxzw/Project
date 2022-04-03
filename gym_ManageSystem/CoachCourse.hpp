#pragma once
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>
#include <list>

#include "ManageDB.hpp"
#include "Coach.hpp"
#include "Course.hpp"
#include "Member.hpp"
#include "Date.hpp"

using namespace std;

struct UserMessage;
struct CoachMessage;
struct CourseMessage;

struct CoachCourseMessage
{
  CoachCourseMessage(int pid,struct UserMessage u,struct CoachMessage c,struct CourseMessage m,int pnum,string pdate,int pstate,string pnote)
    :p_id(pid),um(u),cm(c),cs(m),purchase_num(pnum),purchase_date(pdate),purchase_state(pstate),purchase_note(pnote)
  {}
  int p_id;
  struct UserMessage um;
  struct CoachMessage cm;
  struct CourseMessage cs;
  int purchase_num;
  string purchase_date;
  int purchase_state;
  string purchase_note;
  XPACK(O(p_id,um,cm,cs,purchase_num,purchase_date,purchase_state,purchase_note));
};


class CoachCourse{
  public:
    CoachCourse(){}
    CoachCourse(Member* m,Coach* ch,Course* cs):mb_(m),ch_(ch),cs_(cs)
  {}
    ~CoachCourse(){}

    string CoachCourseMessageQuery(ManageDB*& md_)
    {
      // 将器材的信息存储在链表中                     
      list<CoachCourseMessage> lcm;                     
      lcm.clear();                     
      //统计器材信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define CoachCourseMessageQuery_SQL "select * from CoachCourseList;"
      snprintf(sql,sizeof(sql)-1,CoachCourseMessageQuery_SQL);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : CoachCourseMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int p_id = atoi(row[0]);                                                                                          
        int member_id = atoi(row[1]);                                                                                          
        int coach_id = atoi(row[2]);                                                                                          
        int course_id = atoi(row[3]);                                                                                          
        int purchase_num = atoi(row[4]);    
        string purchase_date(row[5]);
        int purchase_state = atoi(row[6]);    
        string purchase_note(row[7]);

        struct UserMessage um;
        mb_->QueryAllMemberMessage(md_,member_id,um);
        struct CoachMessage cm;
        ch_->QueryAllCoachMessage(md_,coach_id,cm);
        struct CourseMessage cs;
        cs_->QueryAllCourseMessage(md_,course_id,cs);

        CoachCourseMessage cc(p_id,um,cm,cs,purchase_num,purchase_date,purchase_state,purchase_note);
        lcm.push_back(cc);
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
    void test(ManageDB*& md_)
    {
      int member_id = 1;
      int coach_id = 1;
      int course_id = 1;
      struct UserMessage um;
      mb_->QueryAllMemberMessage(md_,member_id,um);
      struct CoachMessage cm;
      ch_->QueryAllCoachMessage(md_,coach_id,cm);
      struct CourseMessage cs;
      cs_->QueryAllCourseMessage(md_,course_id,cs);

      cout << "Member is " << xpack::json::encode(um) << endl;
      cout << endl;
      cout << "Coach is " << xpack::json::encode(cm) << endl;
      cout << endl;
      cout << "Course is " << xpack::json::encode(cs) << endl;
      cout << endl;

    }
  private:
    Member* mb_;
    Coach* ch_;
    Course* cs_;

};
