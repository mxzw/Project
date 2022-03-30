#pragma once

#include <openssl/md5.h>
#include <string.h>
#include <jsoncpp/json/json.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <unordered_map>

#include "tools.hpp"
#include "httplib.h"

using namespace std;
using namespace httplib;


class Session{
  friend class AllInfoUserSession;

  public:
  Session()
  {}
  ~Session()
  {}
  Session(Json::Value& v,int user_id)
  {
    real_str_.clear();

    real_str_ += v["login_email"].asString();
    real_str_ += v["login_passwd"].asString();

    user_id_ = user_id;

  }

  /*
   * SumMd5 ： 生成对应的MD5码(初始化session_id_)
   *  即将real_str_中的内容调用MD5_xxx函数生成对应的md5码，存入session_id中
   *  成功：true
   *  失败：false
   *  int MD5_Init(MD5_CTX *c);初始化MD5码
   *  int MD5_Update(MD5_CTX *c, const void *data, unsigned long len); 通过传入的字符串更新MD5码
   *  int MD5_Final(unsigned char *md, MD5_CTX *c); 获取生成的MD5码
   */ 
  bool SumMd5()
  {
    MD5_CTX c;
    //1.初始化MD5码
    MD5_Init(&c);

    //2.更新和获取MD5码
    if(MD5_Update(&c,real_str_.c_str(),real_str_.size()) != 1)
    {
      return false;
    }

    unsigned char md5[16] = {0};
    if(MD5_Final(md5,&c) != 1)
    {
      return false;
    }

    //3.将生成的MD5码按照16进制的格式存入会话id中
    char tmp[3] = {0};
    char buf[32] = {0};
    for(int i = 0; i < 16; ++i)
    {
      snprintf(tmp,sizeof(tmp)-1,"%02x",md5[i]);
      //cout << tmp;
      strncat(buf,tmp,2);
    }

    session_id_ = buf;
    return true;
  }

  /*
   * GetSessionID :  返回session_id_
   *  本质上在内部调用了SumMd5函数进行初始化
   */ 
  string& GetSessionID()
  {
    SumMd5();
    return session_id_;
  }

  private:
  //当前会话的会话id
  string session_id_;
  //当前会话的原生字符串
  string real_str_;

  int user_id_;

};

//生成了对应的session之后，我们需要一个类将所有用户的session_id存储起来，用于后面的session校验
class AllInfoUserSession
{
  public:
    AllInfoUserSession()
    {
      session_map_.clear();
      pthread_mutex_init(&map_lock_,NULL);
    }

    /*
     * SetSessionInfo ： 将对应的会话id和会话插入到unordered_map中
     */ 
    void SetSessionInfo(string& session_id,Session& sess)
    {
      pthread_mutex_lock(&map_lock_);
      session_map_.insert(make_pair(session_id,sess));
      pthread_mutex_unlock(&map_lock_);
    }

    /*
     * CheckSessionInfo ： 校验对应的会话信息
     *  通过从http请求中拿到对应的sessionID来和当前map中保存的信息进行对比
     *  若map中不存在，则当前会话是不存在的，返回一个小于0的数，否则返回user_id
     */ 
    int CheckSessionInfo(const Request& req)
    {
      //获取用户id
      string session_id = GetSessionID(req);
      if(session_id == "")
        return -1;

      //在map中进行匹配
      Session ss;
      if(GetSessionInfo(session_id,&ss) < 0)
      {
        return -2;
      }

      return ss.user_id_;
    }

    /*
     * GetSessionID：根据传入的req，对Cookie按=进行切割
     * 返回值：返回user_id
     */ 
    string GetSessionID(const Request& req)
    {
      //从请求头中拿到对应的cookie
      string session = req.get_header_value("Cookie");
      //JSESSIONID=xxxxxxx ---> 正常情况
      //JSESSIONID=   ---->未登录用户(异常情况)
      vector<string> sv;
      StringTools::Split(session,"=",sv);
      return sv[sv.size()-1];

    }

    /*
     * GetSessionInfo : 根据传入的user_id，在map中查找对应的Session
     * 若查找不到，则返回-1，成功返回0
     */ 
    int GetSessionInfo(string user_id,Session *ss)
    {
      pthread_mutex_lock(&map_lock_);
      auto it = session_map_.find(user_id);
      if(it == session_map_.end())
      {
        pthread_mutex_unlock(&map_lock_);
        return -1;
      }

      if(ss != nullptr)
      {
        *ss = it->second;
      }

      pthread_mutex_unlock(&map_lock_);
      return 0;
    }


    ~AllInfoUserSession()
    {
      pthread_mutex_destroy(&map_lock_);
    }

  private:
    //为了查找时更为快捷，且要保证不重复，我们在此使用unorder_map对用户会话id进行存储
    unordered_map<string,Session> session_map_;

    //为了保证线程安全，我们还需要一个互斥锁
    // 因为在后面的对战匹配的过程中，我们需要用到会话信息进行校验匹配，这个过程是在多线程环境下走的，
    // 因此就必须要保证线程安全
    pthread_mutex_t map_lock_;

};
