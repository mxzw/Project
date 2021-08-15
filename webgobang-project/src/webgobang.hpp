#pragma once
#include <iostream>
#include <vector>
#include <string>

#include <jsoncpp/json/json.h>
#include "httplib.h"
#include "ManagerDB.hpp"
#include "Session.hpp"
#include "room_player.hpp"

using namespace std;
using namespace httplib;

class WebGoBang{
    public:
        WebGoBang():db_svr_(nullptr),all_sess_(nullptr),pm_(nullptr),rm_(nullptr)
        {
            match_pool_.clear();
            match_pool_num_ = 0;
            pthread_mutex_init(&vec_lock_,NULL);
            pthread_cond_init(&vec_cond_,NULL);
        }   

        ~WebGoBang()
        {
            /*
             * nullptr 和 NULL 的区别
             *  nullptr在C++代表着空指针
             *  NULL 在 C++ 代表着 0 
             */ 
            if(db_svr_)
            {
                delete db_svr_;
                db_svr_ = nullptr;
            }
            if(all_sess_)
            {
                delete all_sess_;
                all_sess_ =nullptr;

            }
            if(pm_)
            {
                delete pm_;
                pm_ = nullptr;
            }

            if(rm_)
            {
                delete rm_;
                rm_ = nullptr;
            }

            pthread_mutex_destroy(&vec_lock_);
            pthread_cond_destroy(&vec_cond_);
        }

        /*
         * Init()函数：
         * 初始化用户管理模块的类指针
         * 初始化数据库管理模块的类指针
         * 初始化房间管理模块的类指针
         * 初始化会话模块的类指针
         */ 
        int Init()
        {
            // 1.创建数据库对象 并连接数据库
            db_svr_ = new ManagerDB();
            if(!db_svr_)
            {
                cout << "new ManagerDB failed" << endl;
                return -1;
            }

            bool flag = db_svr_->ConnectDB();
            if(!flag)
            {
                cout << "ManagerDB connect failed" << endl;
                return -2;
            }

            //2.创建所有session信息的对象
            all_sess_ = new AllInfoUserSession();
            if(!all_sess_)
            {
                return -3;
            }

            //3.创建用户管理的对象
            pm_ = new PlayerManager();
            if(!pm_)
            {
                return -4;
            }
            //4. 创建房间管理对象
            rm_ = new RoomManager();
            if(!rm_)
            {
                return -5;
            }

           return 0; 
        }

        /*
         * StartWebGoBang()函数
         * 对浏览器中的各种请求做出回应
         */
        void StartWebGoBang()
        {
            //用户登录
            http_svr_.Post("/login",[=](const Request& req,Response& res){
                   //1.校验用户(登录页面)所提交的邮箱和密码（与数据库进行比较）
                    cout << req.body << endl; 
                    Json::Reader r;   
                    Json::Value v;   //存储string ：string 的键值对 
                    r.parse(req.body,v); // 用于将请求正文中的内容反序列化（从连续的二进制转为对应键值对）
                                         //cout <<"v[\"email\"]" <<v["email"] << endl; 
                                         //cout <<"v[\"password\"]" <<v["password"] << endl; 
                                         //
                   // 1.1 同数据库进行校验并得到对应user_id
                    int user_id = this->db_svr_->CheckUserExist(v);

                    //1.2 为了防止用户不经过登录，直接访问主页面，我们需要在这里加上一个session会话
                    //      设置MD5码，作为会话id 进行http响应
                    
                    string tmp = "";
                    if(user_id > 0)
                    {
                        Session sess(v,user_id);

                        string session_id = sess.GetSessionID();
                        //回复的会话格式为JSESSIONID = xxxxx
                        tmp += "JSESSIONID=" + session_id;

                        all_sess_->SetSessionInfo(session_id,sess);

                        //将该用户放到用户管理类中进行管理
                        this->pm_->insertPlayer2Map(user_id); 

                    }

                   //2.组织http响应格式使用校验的结果进行响应(使用Json对其进行响应，因为ajax中接收的数据类型为json)
                    Json::Value res_value;
                    // 在前端ajax请求中，要求接收的是一个Json对象，并且该对象中含有status的键值对
                    res_value["status"] = user_id >= 0 ? true : false;


                    //将该Json-Value对象序列化 并进行响应
                    res.body = Serializa(res_value);
                    res.set_header("Set-Cookie",tmp.c_str());
                    res.set_header("content-Type","application/json");
                     
                    });
            //用户注册
            http_svr_.Post("/register",[=](const Request& req,Response& res)
                    {
                        cout << req.body << endl;
                        //1.获取浏览器输入的邮箱和密码
                        Json::Reader r;
                        Json::Value v;
                        r.parse(req.body,v);

                        //2.将邮箱和密码更新在数据库中,并进行响应
                        Json::Value res_value;
                        res_value["status"] = this->db_svr_->AddUser(v);

                        res.body = this->Serializa(res_value);
                        res.set_header("content-Type","application/json");
                    }); 

            //session会话校验 （在js/script.js中有一个ajax请求，请求我们返回user_id
            //                  进行判断，若大于0，则在gobang.html绘制棋盘，否则返回
            //                  index.html页面进行登录）
            http_svr_.Get("/GetUserId",[=](const Request& req,Response& res)
                    {
                        //1.进行会话校验
                        Json::Value res_json;
                        res_json["user_id"]= this->all_sess_->CheckSessionInfo(req);
                        res.body = this->Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    });

            //开始匹配(将用户放到匹配池当中)（当点击开始匹配按钮的时候，ajax会发送一个请求，我们需要将当前这个用户放到匹配池当中进行匹配） 
            http_svr_.Get("/SetMatch",[=](const Request& req,Response& res)
                    {
                        Json::Value res_json; 
                        int user_id = this->all_sess_->CheckSessionInfo(req);
                        if(user_id <= 0)
                        {
                            res_json["status"] = -1;
                        }
                        else
                        {
                           //将当前的用户放到匹配池当中 
                           res_json["status"] = this->PushPlayer2MatchPool(user_id);
                        }
                        res.body = Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    });
            //获取匹配结果(正在匹配的时候，ajax会发送一个请求，判断我们当前的匹配状态) 
            http_svr_.Get("/Match",[=](const Request& req,Response& res)
                    {
                        Json::Value res_json; 
                        int user_id = this->all_sess_->CheckSessionInfo(req);
                        if(user_id <= 0)
                        {
                            res_json["status"] = -1;
                        }
                        else
                        {
                            //使用用户id判断是否该用户有房间号
                            //如果有房间号，则匹配成功
                            //如果没有，则匹配失败

                            //1.获取当前这个用户
                            //  1.1 想要获取用户所持有的棋子
                            //  1.2 想要获取当前用户所在房间号
                            
                           Player r = this-> pm_->GetPlayerInfo(user_id);
                           if(r.room_id_ >= 1000 && r.player_status_ == PLAYING)
                           {
                               //匹配成功
                                res_json["status"] = 1;
                                res_json["room_id"] = r.room_id_;
                                res_json["chess_name"] = r.chess_name_;
                           }
                           else
                           {
                                //匹配失败
                                res_json["status"] = 0;
                           }
                        }
                        res.body = Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    });
            
            //判断当前是否是自己的轮回
            http_svr_.Post("/IsMyTurn",[=](const Request& req,Response& res)
                    {
                        Json::Value res_json; 
                        int user_id = this->all_sess_->CheckSessionInfo(req);
                        if(user_id <= 0)
                        {
                            res_json["status"] = -1;
                        }
                        else
                        {
                            Json::Reader r;
                            Json::Value v;
                            r.parse(req.body,v);

                            int room_id = v["room_id"].asInt();
                            int user_id = v["user_id"].asInt();

                            res_json["status"] = this->rm_->IsMyTurn(room_id,user_id);
                        }
                        res.body = Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    });
            http_svr_.Post("/Step",[=](const Request& req,Response& res)
                    {
                        Json::Value res_json; 
                        int user_id = this->all_sess_->CheckSessionInfo(req);
                        if(user_id <= 0)
                        {
                            res_json["status"] = -1;
                        }
                        else
                        {
                        //    cout << "a:" << user_id << endl; 
                           res_json["status"] = this->rm_->Step(req.body); 
                        }
                        res.body = Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    });
            http_svr_.Post("/GetPeerStep",[=](const Request& req,Response& res)
                    {
                        Json::Value res_json; 
                        int user_id = this->all_sess_->CheckSessionInfo(req);
                        if(user_id <= 0)
                        {
                            res_json["status"] = -1;
                        }
                        else
                        {
                           string peer_step;
                           int ret = this->rm_->GetRoomStep(req.body,peer_step); 
                           if(ret < 0)
                           {
                                res_json["status"] = 0;
                           }
                           else
                           {
                                res_json["status"] = 1;
                                res_json["peer_step"] = peer_step;
                           }
                        }
                        res.body = Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    });  
            http_svr_.Post("/Winner",[=](const Request& req,Response& res )
                    {
                        Json::Value res_json;
                        int user_id = this->all_sess_->CheckSessionInfo(req);
                        if(user_id <= 0)
                        {
                            res_json["status"] = -1;
                        }
                        else
                        {
                            Json::Reader r;
                            Json::Value v;
                            r.parse(req.body,v);

                            int user_id = v["user_id"].asInt();
                            int room_id = v["room_id"].asInt();

                           //1.设置对局的赢家到对应的房间当中 
                          this->rm_->SetRoomWinner(room_id,user_id); 
                           //2.将对局的步骤持久化到（保存）在数据库中
                           //新建一个表，将其步骤结果进行保存
                           /* 表中列的属性
                            * room_id p1 p2 winner start_user step 对局开始时间
                            * 2.1 获取当前返回房间信息
                            * 2.2 将房间信息交给数据库模块进行持久化
                            */ 
                          int ret = this->db_svr_->InsertRoomInfo(this->rm_->GetRoomInfo(room_id));
                          res_json["status"] = ret;
                        }
                        res.body = Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    }); 
            http_svr_.Post("/ReStart",[=](const Request& req,Response& res )
                    {
                        Json::Value res_json;
                        int user_id = this->all_sess_->CheckSessionInfo(req);
                        if(user_id <= 0)
                        {
                            res_json["status"] = -1;
                        }
                        else
                        {
                            int user_id = this->ReaderRequestId(req.body,"user_id");
                            int room_id = this->ReaderRequestId(req.body,"room_id");
                            //1.将用户信息进行重置，将用户状态重置为匹配状态
                            //   游戏房间信息也进行重置
                            this->pm_->ResetUserGameInfo(user_id); 
                            //2.将用户所在房间进行移除，
                            this->rm_->RemoveRoom(room_id);

                            res_json["status"] = 0;
                        }
                        res.body = Serializa(res_json);
                        res.set_header("Content-Type","application/json");
                    });


            pthread_t pid;
            int ret = pthread_create(&pid,NULL,MatchServer,(void*)this);
            if(ret < 0)
            {
                cout << "pthread_create failed" << endl;
                exit(1);
            }


            http_svr_.set_mount_point("/","./www"); // 设置服务器的根目录
            http_svr_.listen("0.0.0.0",18989); //侦听端口为18989，绑定任一地址
        }

        // 匹配线程入口函数
        static void* MatchServer(void* arg)
        {
            pthread_detach(pthread_self());
            WebGoBang* wg = (WebGoBang*)arg;
            
            while(1)
            {
                pthread_mutex_lock(&wg->vec_lock_);
                //从匹配池中进行匹配
                //要操作的是vector，所以首先要加锁，
                //然后还要判断当前正在匹配玩家的数量，如果小于2，则放入等待队列中 
                while(wg->match_pool_num_ < 2)
                {
                    pthread_cond_wait(&wg->vec_cond_,&wg->vec_lock_);
                }

                //匹配池中的人数一定大于等于2
                // 奇数：一定有一个玩家要轮空
                // 偶数：两两进行匹配
                
                vector<int>& iv = wg->match_pool_;
                size_t size = iv.size(); 
                int last_id = -1;
                if(size % 2)
                {
                    //人数是奇数
                    //默认让最后一个人轮空
                    last_id = iv[size-1];
                    size -= 1;
                }

//                for(auto &e : iv)
//                    cout << e << " ";
//                cout << endl;
                for(int i = size - 1;i >= 0; i-=2)
                {
                    int player_one = iv[i];
                    int player_two = iv[i-1];

                    //假设匹配成功
                    int room_id = wg->rm_->CreateRoom(player_one,player_two); 
                    //然后再让当前这两个用户的状态变为PALYING
                    wg->pm_->SetUserStartStatus(player_one,PLAYING);
                    wg->pm_->SetUserStartStatus(player_two,PLAYING);

                    //为该用户分配房间号
                    wg->pm_->SetRoomID(player_one,room_id);
                    wg->pm_->SetRoomID(player_two,room_id);

                    //为用户分配黑棋/白棋
                    wg->pm_->SetUserChessName(player_one,"黑棋");
                    wg->pm_->SetUserChessName(player_two,"白棋");
                    
                    
                    cout << "player1:" << player_one << ", palyer2:" << player_two << ",roomId:"<< room_id << endl;

                }
                if(last_id < -1)
                    wg->PushPlayer2MatchPool(last_id);

                wg->MatchPoolClear();
                pthread_mutex_unlock(&wg->vec_lock_);
            }
            return NULL;
        }

        void MatchPoolClear()
        {
            match_pool_.clear();
            match_pool_num_ = 0;
        }

        /*
         * PushPlayer2MatchPool ： 将当前用户放到匹配池中
         *
         */ 
        int PushPlayer2MatchPool(int user_id)
        {
           // cout << "user_id : " << user_id << endl;
            //1.设置用户的状态为匹配状态，MATCHING
            pm_->SetUserStartStatus(user_id,MATCHING); 
            //2.将用户放到匹配池当中
            pthread_mutex_lock(&vec_lock_);
            match_pool_.push_back(user_id);
            match_pool_num_++;
            pthread_mutex_unlock(&vec_lock_);

            //通知匹配线程工作
            // 使用pthread_cond_signal()也没有什么问题，都是对匹配线程进行通知
            pthread_cond_broadcast(&vec_cond_);
            return 0;
        }

        // 获取http请求中的相关id信息
        int ReaderRequestId(const string& body,const string id)
        {
            Json::Reader r;
            Json::Value v;
            
            r.parse(body,v);

            return v[id].asInt();
        }

        string Serializa(Json::Value& v)
        {
            Json::FastWriter fw;
            return fw.write(v);
        }

    private:
        Server http_svr_;

        ManagerDB* db_svr_;

        AllInfoUserSession* all_sess_;

        PlayerManager* pm_;

        RoomManager* rm_;

        //匹配池： vector，需要匹配之后开始游戏的用户，匹配线程就是通过匹配池进行匹配的
        vector<int> match_pool_;
        //当前正在匹配的人数
        int match_pool_num_;
        //保证线程安全
        pthread_mutex_t vec_lock_;
        //当匹配池中没有用户进行匹配的时候，就将匹配线程放入等待队列中进行等待
        //为了减少CPU的消耗，提升性能
        pthread_cond_t vec_cond_;

};
