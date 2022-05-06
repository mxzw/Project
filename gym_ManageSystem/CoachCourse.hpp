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
  CoachCourseMessage(int pid,struct UserMessage u,struct CoachMessage c,struct CourseMessage m,int pnum,string pdate,int pstate,string pnote,double pmoney)
    :p_id(pid),um(u),cm(c),cs(m),purchase_num(pnum),purchase_date(pdate),purchase_state(pstate),purchase_note(pnote),purchase_money(pmoney)
  {}
  int p_id;
  struct UserMessage um;
  struct CoachMessage cm;
  struct CourseMessage cs;
  int purchase_num;
  string purchase_date;
  int purchase_state;
  string purchase_note;
  double purchase_money;
  XPACK(O(p_id,um,cm,cs,purchase_num,purchase_date,purchase_state,purchase_note,purchase_money));
};


class CoachCourse{
  public:
    CoachCourse(){}
    CoachCourse(Member* m,Coach* ch,Course* cs):mb_(m),ch_(ch),cs_(cs)
  {}
    ~CoachCourse(){}
    //根据id删除教练课程信息
    bool DelCoachCourseMessage(ManageDB*& md_,Json::Value& v)
		{
      int p_id = stoi(v["p_id"].asString());
      char sql[1024]={0};                     
#define DelCoachCourseMessage_SQL "delete from CoachCourseList where p_id = %d;"
      snprintf(sql,sizeof(sql)-1,DelCoachCourseMessage_SQL,p_id);

      return md_->ExecSQL(sql);
					
		}

    //根据id和变更教练姓名修改教练课程信息
    bool UpdateCoachCourseMessage(ManageDB*& md_,Json::Value& v)
    {
      int p_id = stoi(v["p_id"].asString());
      int coach_id = stoi(v["coach_id"].asString());

      char sql[1024]={0};                     
#define UpdateCoachCourseMessage_SQL "update CoachCourseList set coach_id=%d where p_id = %d;"
      snprintf(sql,sizeof(sql)-1,UpdateCoachCourseMessage_SQL,coach_id,p_id);

      return md_->ExecSQL(sql);
      
    }

    //新增一个教练课程信息
    bool AddCoachCourseMessage(ManageDB*& md_,Json::Value& v)
    {
      int member_id = stoi(v["member_id"].asString());
      int coach_id = stoi(v["coach_id"].asString());
      int course_id = stoi(v["course_id"].asString());
      int purchase_num = stoi(v["purchase_num"].asString());
      string purchase_date = v["purchase_date"].asString();
      int purchase_state = stoi(v["purchase_state"].asString());
      string purchase_note = v["purchase_note"].asString();
      double purchase_money = stod(v["purchase_money"].asString());

      char sql[1024]={0};                     
#define AddCoachCourseMessage_SQL "insert CoachCourseList value(null,%d,%d,%d,%d,\'%s\',%d,\'%s\',%lf);"
      snprintf(sql,sizeof(sql)-1,AddCoachCourseMessage_SQL,member_id,coach_id,course_id,purchase_num,purchase_date.c_str(),purchase_state,purchase_note.c_str(),purchase_money);

      return md_->ExecSQL(sql);
    }
    //查询所有的教练课程信息
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

        double purchase_money = cs.course_price * purchase_num;

        CoachCourseMessage cc(p_id,um,cm,cs,purchase_num,purchase_date,purchase_state,purchase_note,purchase_money);
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

    string CoachCourseMessageQuery(ManageDB*& md_,int mid)
    {
      // 将器材的信息存储在链表中                     
      list<CoachCourseMessage> lcm;                     
      lcm.clear();                     
      //统计器材信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define UserCoachCourseMessageQuery_SQL "select * from CoachCourseList where member_id=%d;"
      snprintf(sql,sizeof(sql)-1,UserCoachCourseMessageQuery_SQL,mid);     
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

        double purchase_money = cs.course_price * purchase_num;

        CoachCourseMessage cc(p_id,um,cm,cs,purchase_num,purchase_date,purchase_state,purchase_note,purchase_money);
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


    //根据pid查询返回一行的教练课程信息给前端
    string OneRowMessageQuery(ManageDB*& md_,int& pid)
    {
      char sql[1024]={0};                     
#define OneRowMessageQuery_SQL "select * from CoachCourseList where p_id = %d;"
      snprintf(sql,sizeof(sql)-1,OneRowMessageQuery_SQL,pid);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : CoachCourseMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      MYSQL_ROW row = mysql_fetch_row(res);
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
      double purchase_money = cs.course_price * purchase_num;

      CoachCourseMessage cc(p_id,um,cm,cs,purchase_num,purchase_date,purchase_state,purchase_note,purchase_money);

      //统计数据的个数
      mysql_free_result(res);

      //转成json
      return "[" +  xpack::json::encode(cc) + "]";
    }
    //根据用户、课程、教练id检索教练课程信息
    string CoachCourseMessageSearch(ManageDB*& md_,Json::Value& v)
    {
      int mid = stoi(v["member_id"].asString());
      int cid = stoi(v["coach_id"].asString());
      int sid = stoi(v["course_id"].asString());

      string sql = "select * from CoachCourseList where 1=1";
			if(mid != -1)
				sql += " and member_id=" + to_string(mid);
			if(cid != -1)
				sql += " and coach_id=" + to_string(cid);
			if(sid != -1)
				sql += " and course_id=" + to_string(sid);
			
			sql += ";";

      list<CoachCourseMessage> lcm;                     
      lcm.clear();                     
      //统计器材信息的个数                     
      int countNum = 0;                     

      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql.c_str(),res))          
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

        double purchase_money = cs.course_price * purchase_num;

        CoachCourseMessage cc(p_id,um,cm,cs,purchase_num,purchase_date,purchase_state,purchase_note,purchase_money);
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
    string incomeInfo(ManageDB*& md_)
    {
      string date[12] = {"2022-01","2022-02","2022-03","2022-04","2022-05","2022-06","2022-07","2022-08","2022-09","2022-10","2022-11","2022-12"};

      vector<double> vd;
      for(int i = 0; i < 12; ++i)
      {
        string sql = "select sum(purchase_money) from CoachCourseList where purchase_date like \'" + date[i] + "%\';";
        cout << "sql is " << sql << endl;
        MYSQL_RES* res;                                                  
        if(!md_->ExecSQL(sql.c_str(),res))          
        {                                              
          cout << "ExecSQL failed : incomeInfo, sql is " << sql << endl;
          mysql_free_result(res);                                                             
          return "";                                  
        }                                
        MYSQL_ROW row = mysql_fetch_row(res);
        if(row[0] == NULL)
        {
          vd.push_back(0.0);
        }
        else 
        {
          vd.push_back(stod(string(row[0])));
        }
        mysql_free_result(res);                                                             
        
      }
      return xpack::json::encode(vd);
    }

  private:
    Member* mb_;
    Coach* ch_;
    Course* cs_;

};
