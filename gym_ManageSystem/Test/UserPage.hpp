#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include "httplib.h"
#include "AdminPage.hpp"

using namespace httplib;
using namespace std;

class UserManageSystem
{
	public:
		UserManageSystem():md_(nullptr),all_sess_(nullptr),mb_(nullptr),ch_(nullptr),cs_(nullptr),eq_(nullptr),cc_(nullptr)
	{}
		~UserManageSystem(){
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
		void StartUserSys()
		{
			//查询所有教练信息                  
			http_svr_.Post("/UserCoachQuery",[=](const Request& req, Response& res)
					{                     
					res.body = this->ch_->CoachMessageQuery(md_);                        
					res.set_header("content-Type","application/json;charset=UTF-8");    
					});

			http_svr_.set_mount_point("/","./www");
			http_svr_.listen("0.0.0.0",18989);
		}
	private:
		Server http_svr_;       
		ManageDB* md_;      
		AllInfoUserSession* all_sess_;
		Member* mb_;                         
		Coach* ch_;         
		Course* cs_;  
		Equipment* eq_;
		CoachCourse* cc_;


};
