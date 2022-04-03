#pragma once
#include <unistd.h>
#include <pthread.h>

#include <iostream>

#include "httplib.h"
#include "ManageDB.hpp"
#include "Session.hpp"

//这三个类在CoachCourse中包含了
//#include "Member.hpp"
//#include "Coach.hpp"
//#include "Course.hpp"
#include "Equipment.hpp"
#include "CoachCourse.hpp"

using namespace std;
using namespace httplib;


class GymManageSystem
{
  public:
    GymManageSystem():md_(nullptr),all_sess_(nullptr),mb_(nullptr),ch_(nullptr),cs_(nullptr),eq_(nullptr),cc_(nullptr)
    {
    }
    ~GymManageSystem()
    {
      if(md_)
        delete md_;
      if(all_sess_)
        delete all_sess_;
      if(mb_)
        delete mb_;
      if(ch_)
        delete ch_;
      if(cs_)
        delete cs_;
      if(eq_)
        delete eq_;
      if(cc_)
        delete cc_;
    }

    /*
     *InitMember(): 作用是将其他类的指针或数据结构进行初始化
     *
     */ 
    int InitMember()
    {
      md_ = new ManageDB(); 
      if(md_ == nullptr)
        return -1;
      if(md_->ConnectDB() == false)
      {
        cout << "db connect failed" << endl;
        delete md_;
        return -2;
      }
      all_sess_ = new AllInfoUserSession();
      if(all_sess_ == nullptr)
      {
        return -1;
      }

      mb_ = new Member(); 
      if(mb_ == nullptr)
        return -1;

      ch_ = new Coach();
      if(ch_ == nullptr)
        return -1;

      cs_ = new Course();
      if(cs_ == nullptr)
        return -1;

      eq_ = new Equipment();
      if(eq_ == nullptr)
        return -1;
      cc_ = new CoachCourse(mb_,ch_,cs_);
      if(cc_ == nullptr)
        return -1;

      return 0;
    }

    //直接使用httplib库中的函数，将Linux作为静态服务器绑定
    //set_mount_point() 和 listen()
    void StartGymSys()
    {
    //登录
     http_svr_.Post("/login",[=](const Request& req,Response& res)
         {
           cout << req.body << endl;
           Json::Reader r;
           Json::Value v;
           r.parse(req.body,v);
           int user_id  = this->md_->CheckUserIsExist(v);

           int user_attributes = this->md_->GetUserAttributes(user_id);
           //为用户和管理员均创建一个Cookie
           if(user_id >= 0)
           {
              Session sess(v,user_id);
              string sessionID = sess.GetSessionID();
              string tmp = "JSESSIONID="+ sessionID;
              //将管理员,用户的session放在map中，用于在后台界面进行验证
              all_sess_->SetSessionInfo(sessionID,sess);

              res.set_header("Set-Cookie",tmp.c_str());
            
           }
           Json::Value res_value;
           res_value["status"] = user_id > 0 ? true : false ;
           res_value["user_attributes"] = user_attributes;
           res.body = this->Serializa(res_value);
           res.set_header("content-Type","application/json");
         }
         );
     //注册
     http_svr_.Post("/register",[=](const Request& req,Response& res)
         {
            Json::Reader r;
            Json::Value v;
            r.parse(req.body,v);

            int add_res = this->md_->AddUser(v);
            cout << "add_res is : " <<add_res << endl;
            Json::Value res_value;
            res_value["status"] = add_res;
            res.body = this->Serializa(res_value);
            res.set_header("content-Type","application");

         }
         );
     //修改密码
     http_svr_.Post("/updPasswd",[=](const Request& req,Response& res)
         {
            Json::Reader r;
            Json::Value v;
            r.parse(req.body,v);
            //首先根据浏览器传来的Cookie，获取对应的user_id
            int user_id = this->all_sess_->CheckSessionInfo(req);
            
            Json::Value res_value;
            res_value["status"] = this->md_->ModifyPasswd(v,user_id);
            res.body = this->Serializa(res_value);
            res.set_header("content-Type","application");

         });
    //验证是否为管理员用户
    http_svr_.Get("/GetUserAttributes",[=](const Request& req,Response res)
        {
          int user_id = this->all_sess_->CheckSessionInfo(req);
          Json::Value res_value;
          res_value["user_attributes"] = this->md_->GetUserAttributes(user_id);
          res.body = this->Serializa(res_value);
          res.set_header("content-Type","application");
          
        });

    //查询会员卡类型
    http_svr_.Post("/CardQuery",[=](const Request& req,Response& res)
        {
          res.body = this->mb_->MemberCardQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });

    
    //MemberQuery  查询用户的所有信息，返回给bootstrap 绘制表格
    http_svr_.Post("/MemberQuery",[=](const Request& req, Response& res)
        {
          res.body = this->mb_->MemberMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });

    //MemberUpdate 修改用户的信息
    http_svr_.Post("/MemberUpdate",[=](const Request& req, Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          if(!this->mb_->UpdateMemberMessage(md_,req_value))
            cout << "/MemberUpdate ----> update failed" << endl;
          else
          {
            cout << "UpdateMemberMessage success" << endl;
            res.body = this->mb_->MemberMessageQuery(md_);

          }

          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //MemberSearch 搜素用户信息
    http_svr_.Post("/MemberSearch",[=](const Request& req, Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);

          res.body = this->mb_->MemberMessageSearch(md_,req_value);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //添加一个新用户
    http_svr_.Post("/AddMember",[=](const Request& req, Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);

          if(!this->mb_->AddMember(md_,req_value))
            cout << "/AddMember ---> add failed" << endl;
          else
            res.body = this->mb_->MemberMessageQuery(md_);
            
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //删除一个用户
    http_svr_.Post("/DelMember",[=](const Request& req, Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          if(!this->mb_->DelMember(md_,req_value))
            cout << "/DelMember ---> add failed" << endl;
          else
            res.body = this->mb_->MemberMessageQuery(md_);
            
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //确定会员卡对应金额
    http_svr_.Post("/QueryCardMoney",[=](const Request& req, Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);

          Json::Value v;
          v["type_money"] = this->mb_->QueryCardMoney(md_,req_value);
          res.body = this->Serializa(v);
          res.set_header("content-Type","application");
        });
    //用户续卡操作的实现
    http_svr_.Post("/UpdateMemberCardMessage",[=](const Request& req, Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          int flag = 1;
          // 1. 根据member_id获取对应的会员卡类型的天数，并将其加到increase_day这一列中
          int member_id = stoi(req_value["member_id"].asString());
          if(!this->mb_->UpdateMemberIncreaseDay(md_,member_id))
          {
            cout << "UpdateMemberIncreaseDay failed !" << endl;
            flag = -1;
          }
          
            // 2. 根据type_id 修改对应用户的会员卡类型
          int type_id = stoi(req_value["type_id"].asString());
          if(!this->mb_->UpdateMemberCardType(md_,member_id,type_id))
          {
            cout << "UpdateMemberCardType failed" << endl;
            flag = -1;
          }
            // 3.将结果返回给前端
          if(flag)
            res.body = this->mb_->MemberMessageQuery(md_);

          res.set_header("content-Type","application/json;charset=UTF-8");
            
        });
    //根据会员卡名称模糊搜素会员卡所有信息
    http_svr_.Post("/MemberCardSearch",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          string type_name = req_value["type_name"].asString();
          res.body = this->mb_->CardMessageSearch(md_,type_name);
          res.set_header("content-Type","application/json;charset=UTF-8");
          
        });
    //根据会员卡id搜索会员卡的所有信息
    http_svr_.Post("/IdToCardMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          int type_id = stoi(req_value["type_id"].asString());
          res.body = this->mb_->IdToCardMessage(md_,type_id);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });
    //修改会员卡的信息
    http_svr_.Post("/UpdateCardMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->mb_->UpdateCardMessage(md_,req_value))
            cout << "UpdateCardMessage failed ! " << endl;
          else 
            res.body = this->mb_->MemberCardQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });
    //新增一个会员卡
    http_svr_.Post("/AddCardMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->mb_->AddCardMessage(md_,req_value))
            cout << "UpdateCardMessage failed ! " << endl;
          else 
            res.body = this->mb_->MemberCardQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });
    //删除一个会员卡
    http_svr_.Post("/DelCardMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->mb_->DelCardMessage(md_,req_value))
            cout << "UpdateCardMessage failed ! " << endl;
          else 
            res.body = this->mb_->MemberCardQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });

    //查询所有教练信息
    http_svr_.Post("/CoachQuery",[=](const Request& req, Response& res)
        {
          res.body = this->ch_->CoachMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });

    //根据教练名称模糊搜索教练信息
    http_svr_.Post("/CoachNameSearch",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          string coach_name = req_value["coach_name"].asString();
          string str = "";
          int flag = 30;
          //防止请求过多造成程序崩溃，导致系统不响应
          while(str.compare("")==0 && flag)
          {
              str = this->ch_->CoachMessageSearch(md_,coach_name);
              flag--;
          }
          res.body = str;
          res.set_header("content-Type","application/json;charset=UTF-8");
          
        });
    //新增一个教练信息
    http_svr_.Post("/AddCoachMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->ch_->AddCoachMessage(md_,req_value))
            cout << "AddCoachMessage failed ! " << endl;
          else 
            res.body = this->ch_->CoachMessageQuery(md_);

          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //根据教练id查询相应教练信息
    http_svr_.Post("/IdToCoachMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          int coach_id = stoi(req_value["coach_id"].asString());
          res.body = this->ch_->IdToCoachMessage(md_,coach_id);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });
    //修改教练信息
    http_svr_.Post("/UpdateCoachMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->ch_->UpdateCoachMessage(md_,req_value))
            cout << "UpdateCoachMessage failed ! " << endl;
          else 
            res.body = this->ch_->CoachMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
          });
    //删除教练信息
    http_svr_.Post("/DelCoachMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->ch_->DelCoachMessage(md_,req_value))
            cout << "DelCoachMessage failed ! " << endl;
          else 
            res.body = this->ch_->CoachMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });
    //查询所有课程信息
    http_svr_.Post("/CourseQuery",[=](const Request& req, Response& res)
        {
          res.body = this->cs_->CourseMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
   
    //根据课程名称模糊搜索课程信息
    http_svr_.Post("/CourseNameSearch",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          string course_name = req_value["course_name"].asString();
          string str = "";
          int flag = 30;
          //防止请求过多造成程序崩溃，导致系统不响应
          while(str.compare("") == 0 && flag)
          {
              str = this->cs_->CourseMessageSearch(md_,course_name);
              flag--;
          }
          res.body = str;
          res.set_header("content-Type","application/json;charset=UTF-8");
          
        });
    //新增一个课程信息
    http_svr_.Post("/AddCourseMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->cs_->AddCourseMessage(md_,req_value))
            cout << "AddCourseMessage failed ! " << endl;
          else 
            res.body = this->cs_->CourseMessageQuery(md_);

          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //根据课程id查询相应课程信息
    http_svr_.Post("/IdToCourseMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          int course_id = stoi(req_value["course_id"].asString());
          res.body = this->cs_->IdToCourseMessage(md_,course_id);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //修改课程信息
    http_svr_.Post("/UpdateCourseMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->cs_->UpdateCourseMessage(md_,req_value))
            cout << "UpdateCourseMessage failed ! " << endl;
          else 
            res.body = this->cs_->CourseMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
          });
    //删除课程信息
    http_svr_.Post("/DelCourseMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->cs_->DelCourseMessage(md_,req_value))
            cout << "DelCourseMessage failed ! " << endl;
          else 
            res.body = this->cs_->CourseMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });
        //查询所有器材信息
    http_svr_.Post("/EquipmentQuery",[=](const Request& req, Response& res)
        {
          res.body = this->eq_->EquipmentMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
   
    //根据器材名称模糊搜索器材信息
    http_svr_.Post("/EquipmentNameSearch",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          string Equipment_name = req_value["eq_name"].asString();
          string str = "";
          int flag = 30;
          //防止请求过多造成程序崩溃，导致系统不响应
          while(str.compare("") == 0 && flag)
          {
              str = this->eq_->EquipmentMessageSearch(md_,Equipment_name);
              flag--;
          }
          res.body = str;
          res.set_header("content-Type","application/json;charset=UTF-8");
          
        });
    //新增一个器材信息
    http_svr_.Post("/AddEquipmentMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->eq_->AddEquipmentMessage(md_,req_value))
            cout << "AddEquipmentMessage failed ! " << endl;
          else 
            res.body = this->eq_->EquipmentMessageQuery(md_);

          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //根据器材id查询相应器材信息
    http_svr_.Post("/IdToEquipmentMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          
          int Equipment_id = stoi(req_value["eq_id"].asString());
          res.body = this->eq_->IdToEquipmentMessage(md_,Equipment_id);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    //修改器材信息
    http_svr_.Post("/UpdateEquipmentMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->eq_->UpdateEquipmentMessage(md_,req_value))
            cout << "UpdateEquipmentMessage failed ! " << endl;
          else 
            res.body = this->eq_->EquipmentMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
          });
    //删除器材信息
    http_svr_.Post("/DelEquipmentMessage",[=](const Request& req,Response& res)
        {
          Json::Reader r;
          Json::Value req_value;
          r.parse(req.body,req_value);
          if(!this->eq_->DelEquipmentMessage(md_,req_value))
            cout << "DelEquipmentMessage failed ! " << endl;
          else 
            res.body = this->eq_->EquipmentMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");

        });
    //将所有的教练课程信息返回给前端
    http_svr_.Post("/CoachCourseQuery",[=](const Request& req,Response& res)
        {
          res.body = this->cc_->CoachCourseMessageQuery(md_);
          res.set_header("content-Type","application/json;charset=UTF-8");
        });
    
      //绑定地址
      http_svr_.set_mount_point("/","./www");
      http_svr_.listen("0.0.0.0",18989);
    }
    //用于一些测试
    void test()
    {
      cc_->test(md_);
      cout << endl;
      cout << cc_->CoachCourseMessageQuery(md_) << endl;
    }

    string Serializa(Json::Value &v)
    {
      Json::FastWriter fw;
      return fw.write(v);
    }
  private:
    //httplib库中的类，可以实现和浏览器进行http通信
    Server http_svr_;
    ManageDB* md_;
    AllInfoUserSession* all_sess_;
    Member* mb_;
    Coach* ch_;
    Course* cs_;
    Equipment* eq_;
    CoachCourse* cc_;

};
