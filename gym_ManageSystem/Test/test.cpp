#include <iostream>
#include <vector>
#include <list>

#include "../httplib.h"
#include "../Date.hpp"
#include "../tools.hpp"
//#include "../Member.hpp"

#include "./FdogSerialize-master/FStruct.h"
#include "./xpack-master/json.h"

#define Serialize_type_judgment_all\    
Serialize_type_judgment(student)\    
Serialize_type_judgment(teacher)\ 

using namespace httplib;
using namespace std;

typedef struct UserMessage     
{                                                

  UserMessage(int mid,string mname,string mphone,int msex,int mage,int mtypeid,string ldate,int mstate,string edate)  
    :member_id(mid),member_name(mname),member_phone(mphone),member_sex(msex), member_age(mage),  
    member_typeid(mtypeid),login_date(ldate),member_state(mstate),expire_date(edate)  
  {}                                                                                                      

  int member_id;                   
  string member_name;               
  string member_phone;                
  int member_sex;                     
  int member_age;                        
  int member_typeid;              
  string login_date;                    
  int member_state;  
  string expire_date;               
  XPACK(O(member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state,expire_date));
}; 

void test5()
{
  list<UserMessage> lum;
  UserMessage um1(1,"zs","12321312",1,21,1,"2021-2-1",1,"2022-2-1");
  UserMessage um2(1,"qgdsa","12321312",1,22,0,"2031-2-1",1,"2032-2-1");
  UserMessage um3(1,"llsadas","12321312",1,21,1,"2021-2-1",1,"2022-2-1");
  UserMessage um4(1,"lkjhjh","12321312",1,21,1,"2021-2-1",1,"2022-2-1");
  lum.push_back(um1);
  lum.push_back(um2);
  lum.push_back(um3);
  lum.push_back(um4);
  string res = xpack::json::encode(lum);
  cout << res << endl;

}

void test1()
{
  Date d("2015-01-02");
  d.show();

  d += 30;
  cout << d.ToString() << endl;

  cout << "当前系统日期" << d.GetNowDate() << endl;

}


void test2()
{

  list<UserMessage> lum;
  UserMessage um1(1,"zs","12321312",1,21,1,"2021-2-1",1,"2022-2-1");
  UserMessage um2(1,"qgdsa","12321312",1,22,0,"2031-2-1",1,"2032-2-1");
  UserMessage um3(1,"llsadas","12321312",1,21,1,"2021-2-1",1,"2022-2-1");
  UserMessage um4(1,"lkjhjh","12321312",1,21,1,"2021-2-1",1,"2022-2-1");
  lum.push_back(um1);
  lum.push_back(um2);
  lum.push_back(um3);
  lum.push_back(um4);
  string res;
  REGISTEREDMEMBER(UserMessage,member_id,member_name,member_phone,member_sex,member_age,member_typeid,login_date,member_state,expire_date);
  //REGISTEREDMEMBER(exp);
  Fdog::FJson(res, um1);
//  cout<< res << endl;

/*  vector<string> vs;
  vs.push_back(res);
  vs.push_back(res);
  vs.push_back(res);
  vs.push_back(res);*/
  string tmp = res + res + res;
  cout << tmp << endl;
  string res2 = "";
  Fdog::FJson(res2, tmp);
  cout<< res2 << endl;


}

void test4()
{
      string str = "{\"total\":1,\"rows\":[{\"member_id\":235,\"member_name\":\"211\",\"member_phone\":\"18861822419\",\"member_sex\":1,\"member_age\":22,\"login_      date\":\"2022-03-29\",\"member_typeid\":{\"typeId\":1,\"typeName\":\"季卡\",\"typeciShu\":20,\"typeDay\":90,\"typemoney\":255.0},\"member_state\":1,\"expire_date\":\"      2025-03-23\"}]}";
      cout << str << endl;
}
/*
void test3()
{
  vector<string> vs;
  vs.push_back("{"member_id":1,"member_sex":1,"member_age":21,"member_typeid":1,"member_state":1}");
  vs.push_back("{"member_id":2,"member_sex":1,"member_age":21,"member_typeid":1,"member_state":1}");
  vs.push_back("{"member_id":3,"member_sex":1,"member_age":21,"member_typeid":1,"member_state":1}");
  vs.push_back("{"member_id":4,"member_sex":1,"member_age":21,"member_typeid":1,"member_state":1}");
  string res = "";
  Fdog::FJson(res, vs);
  cout << res<<endl;

}*/

int main()
{
  test5();
  return 0;
}
