#include <unistd.h>

#include <jsoncpp/json/json.h>

#include <iostream>
#include <string>
#include <list>

#include "ManageDB.hpp"
#include "./xpack-master/json.h"


using namespace std;

struct CourseMessage
{
  CourseMessage(int cid=0,string cname="",double cmoney=0,string cdesc="")
    :course_id(cid),course_name(cname),course_price(cmoney),course_desc(cdesc)
  {}
  CourseMessage(const CourseMessage& cm)
  {
    course_id = cm.course_id;
    course_name = cm.course_name;
    course_price = cm.course_price;
    course_desc = cm.course_desc;
  }
  int course_id;
  string course_name;
  double course_price;
  string course_desc;
  XPACK(O(course_id,course_name,course_price,course_desc));
};

class Course{
  public:
    Course(){}
    ~Course(){}

    //查询所有的课程信息
    string CourseMessageQuery(ManageDB*& md_)
    {
      // 将用户的信息存储在链表中                     
      list<CourseMessage> lcm;                     
      lcm.clear();                     
      //统计用户信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define CourseMessageQuery_SQL "select * from CourseInfo;"
      snprintf(sql,sizeof(sql)-1,CourseMessageQuery_SQL);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : CourseMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int course_id = atoi(row[0]);                                                                                          
        string course_name(row[1]);
        double course_price = stod(string(row[2]));
        string course_desc = row[3] == NULL ? "" : row[3];

        CourseMessage cm(course_id,course_name,course_price,course_desc);
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
    void QueryAllCourseMessage(ManageDB*& md_,int cid,struct CourseMessage& cs)
    {
      char sql[1024]={0};                     
#define QueryAllCourseMessage_SQL "select * from CourseInfo where course_id=%d;"
      snprintf(sql,sizeof(sql)-1,QueryAllCourseMessage_SQL,cid);
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : CourseMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return ;                                  
      }                                
      MYSQL_ROW row = mysql_fetch_row(res);
      int course_id = atoi(row[0]);                                                                                          
      string course_name(row[1]);
      double course_price = stod(string(row[2]));
      string course_desc = row[3] == NULL ? "" : row[3];

      cs = CourseMessage(course_id,course_name,course_price,course_desc);
      mysql_free_result(res);
    }

    //根据课程名来模糊搜索课程信息
    string CourseMessageSearch(ManageDB*& md_,string course_name)
    {
      // 将用户的信息存储在链表中                     
      list<CourseMessage> lcm;                     
      lcm.clear();                     
      //统计用户信息的个数                     
      int countNum = 0;                     

      char sql[1024]={0};                     
#define CourseMessageSearch_SQL "select * from CourseInfo where course_name like \'%%%s%%\';"
      snprintf(sql,sizeof(sql)-1,CourseMessageSearch_SQL,course_name.c_str());
      MYSQL_RES* res;                                                  
      if(!md_->ExecSQL(sql,res))          
      {                                              
        cout << "ExecSQL failed : CourseMessageQuery, sql is " << sql << endl;
        mysql_free_result(res);                                                             
        return "";                                  
      }                                
      //unsigned int len = mysql_num_fields(res);
      //查出来的每一行数据存储在row中                                 
      MYSQL_ROW row;                                       
      while((row = mysql_fetch_row(res))!=NULL)
      {                                                             
        int course_id = atoi(row[0]);                                                                                          
        string course_name(row[1]);
        double course_price = stod(string(row[2]));
        string course_desc = row[3] == NULL ? "" : row[3];

        CourseMessage cm(course_id,course_name,course_price,course_desc);
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

    //新增课程信息
    bool AddCourseMessage(ManageDB*& md_,Json::Value& v)    
    {    
      string course_name = v["course_name"].asString();                         
      double course_price = stod(v["course_price"].asString());    
      string course_desc = v["course_desc"].asString(); 

      char sql[1024]={0};         
#define AddCourseMessage_SQL "insert into CourseInfo values(null,\'%s\',%lf,\'%s\');"
      snprintf(sql,sizeof(sql)-1,AddCourseMessage_SQL,course_name.c_str(),course_price,course_desc.c_str());    

      return md_->ExecSQL(sql);     
    } 
    // 根据课程id查询课程信息
    string IdToCourseMessage(ManageDB*& md_,int tid)
    {
      char sql[1024]={0};
#define IdToCourseMessage_SQL "select * from CourseInfo where course_id = %d;"
      snprintf(sql,sizeof(sql)-1,IdToCourseMessage_SQL,tid);
      MYSQL_RES* res;
      if(!md_->ExecSQL(sql,res))
      {
        cout << "ExecSQL failed : MemberCoachQuery, sql is " << sql << endl;
        mysql_free_result(res);
        return "";
      }
      MYSQL_ROW row = mysql_fetch_row(res);
      int course_id = atoi(row[0]);                                                                                          
      string course_name(row[1]);
      double course_price = stod(string(row[2]));
      string course_desc = row[3] == NULL ? "" : row[3];

      CourseMessage cm(course_id,course_name,course_price,course_desc);

      mysql_free_result(res);

      //转成json
      string str = xpack::json::encode(cm);
      return str;
    }
    //修改课程信息
    bool UpdateCourseMessage(ManageDB*& md_,Json::Value& v)
    {
      int course_id = stoi(v["course_id"].asString());
      string course_name = v["course_name"].asString();                         
      double course_price = stod(v["course_price"].asString());
      string course_desc = v["course_desc"].asString(); 
      

      char sql[1024]={0};
#define UpdateCourseMessage_SQL "update CourseInfo set course_name =\'%s\',course_price=%lf,course_desc = \'%s\' where course_id=%d;"
      snprintf(sql,sizeof(sql)-1,UpdateCourseMessage_SQL,course_name.c_str(),course_price,course_desc.c_str(),course_id);
      return md_->ExecSQL(sql);
    }

    //删除课程信息
    bool DelCourseMessage(ManageDB*& md_,Json::Value& v)
    {
      int course_id = stoi(v["course_id"].asString());
      char sql[1024]={0};
#define DelCourseMessage_SQL "delete from CourseInfo where course_id=%d;"
      snprintf(sql,sizeof(sql)-1,DelCourseMessage_SQL,course_id);
      return md_->ExecSQL(sql);
    }

};
