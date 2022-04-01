#pragma once
#include <unistd.h>
#include <pthread.h>

#include <iostream>

#include "httplib.h"
#include "ManageDB.hpp"
#include "Session.hpp"
#include "Member.hpp"

using namespace std;
using namespace httplib;


class GymManageSystem
{
  public:
    GymManageSystem():md_(nullptr),all_sess_(nullptr),mb_(nullptr)
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

     

      //绑定地址
      http_svr_.set_mount_point("/","./www");
      http_svr_.listen("0.0.0.0",18989);
    }
    //用于一些测试
    void test()
    {
      //Json::Value res_value;
      string v = this->mb_->MemberMessageQuery(md_);
      Json::FastWriter fw;
      
      cout <<  "MemberMessageQueryber is " << v << endl;
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

};
