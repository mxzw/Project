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
  CourseMessage(int cid=0,string cname="",double cmoney=0)
    :course_id(cid),course_name(cname),course_price(cmoney)
  {}
  int course_id;
  string course_name;
  double course_price;
  XPACK(O(course_id,course_name,course_price));
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

        CourseMessage cm(course_id,course_name,course_price);
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

        CourseMessage cm(course_id,course_name,course_price);
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




};
