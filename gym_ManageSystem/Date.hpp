#pragma once

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <ctime>
#include "tools.hpp"
using namespace std;


class Date{
  public:
    // 获取某年某月的天数
    int GetMonthDay(int year, int month) {
      static int days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      int day = days[month];
      if (month == 2
          && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        day += 1;
      }
      return day;
    }

    // 全缺省的构造函数
    Date(int year = 1900, int month = 1, int day = 1) {
      //首先判断日期的格式是否正确
      if (year < 1900 || month < 1 || month > 12 || day < 1 || day > GetMonthDay(year, month)) {
        cout << "日期格式不合格！！！" << endl;
        cout << "日期将被初始化为默认日期：1900/1/1" << endl;
        this->_year = 1900;
        this->_month = 1;
        this->_day = 1;
      }
      this->_year = year;
      this->_month = month;
      this->_day = day;
    }
    //string的格式为 yy-mm-dd 
    Date(string date)
    {
      vector<string> sv;
      StringTools::Split(date,"-",sv);
      this->_year = stoi(sv[0]);
      this->_month = stoi(sv[1]);
      this->_day = stoi(sv[2]);
    }

    void show()const
    {
      cout << _year << "-" << _month << "-" << _day << endl;
    }
    //获取当前系统时间 使用ctime库中的函数
    static string GetNowDate() 
    {
      string res="";
      // 基于当前系统的当前日期/时间,即为1970 到目前经过秒数
      time_t now = time(0);

      tm *ltm = localtime(&now);

      
      string mo = to_string(1 + ltm->tm_mon);
      if(mo.length() < 2)
        mo = "0" + mo;
      string dy = to_string(ltm->tm_mday);
      if(dy.length() < 2)
        dy = "0" + dy;

      res += to_string(1900 + ltm->tm_year) + "-"; //年
      res += mo + "-"; //月
      res += dy; //日

      return res;

    }
    //将当前时间字符串化
    string ToString() const
    {
      string mo = to_string(_month);
      if(mo.length() < 2)
        mo = "0" + mo;
      string dy = to_string(_day);
      if(dy.length() < 2)
        dy = "0" + dy;
      return "" + to_string(_year) + "-" + mo + "-" + dy;
    }

    // 拷贝构造函数
    // d2(d1)
    Date(const Date &d) {
      this->_year = d._year;
      this->_month = d._month;
      this->_day = d._day;
    }

    // 赋值运算符重载
    // d2 = d3 -> d2.operator=(&d2, d3)
    Date &operator=(const Date &d) {
      if (this != &d) {
        this->_year = d._year;
        this->_month = d._month;
        this->_day = d._day;
      }

      return *this;
    }

    // 析构函数
    ~Date() { }

    // 日期+=天数
    /*
     * 可分为两种情况
     * 1. day为负值，即 a += -b
     * 2. day为正值，即 a += b
     */
    Date &operator+=(int day) {
      if (day < 0) {
        //当day为负时，可直接转化为 *this —= -day 的形式
        return *this -= -day;
      }
      /*其次，当day为正的时候
       *还要进行判断，对day进行相加，超过该月最大天数的时候，则需要对月份+1
       *简而言之就是要进行日期格式的规格化
       */
      _day += day;

      //对日期进行规格化
      while (_day > GetMonthDay(_year, _month)) {
        //首先设置day的值,只需用它减去本月最大天数即可
        _day -= GetMonthDay(_year, _month);

        //然后再设置并判断月份的值
        _month++;
        if (_month == 13) {
          //走到这里，说明要设置年份的值，并初始化月份
          _year++;
          _month = 1;
        }
      }

      return *this;
    }

    // 日期+天数
    Date operator+(int day) {
      Date tmp(this->_year, this->_month, this->_day);
      tmp._day = this->_day + day;
      return tmp;

    }

    // 日期-天数
    Date operator-(int day) {
      Date tmp(this->_year, this->_month, this->_day);
      tmp._day = this->_day - day;
      return tmp;
    }


    // 日期-=天数
    /*
     * 同 += 操作，分两种情况
     * a -= b
     * a -= -b
     */
    Date &operator-=(int day) {
      if (day < 0) {
        return *this += -day;
      }

      this->_day -= day;

      //日期规格化
      while (_day <= 0) {
        --_month;
        if (_month == 0) {
          --_year;
          _month = 12;
        }
        _day += GetMonthDay(_year,_month);
      }
      return *this;
    }

    // 前置++
    //即就是 d.operator++(&d)
    //前置++就是先++再返回
    Date &operator++() {
      //不能用++，否则会无限递归
      *this += 1;
      return *this;
    }

    // 后置++
    //即就是 d.operator++(&d, 0)
    //后置++就是将值返回之后再++
    Date operator++(int) {
      return *this;
      Date tmp(*this);
      *this += 1;
      return tmp;
    }

    // 后置--
    // d.operator--(&d,0);
    Date operator--(int)
    {
      Date tmp(*this);
      *this -= 1;
      return tmp;
    }

    // 前置--
    //d.operator--(&d)
    Date &operator--()
    {
      *this -= 1;
      return *this;
    }

    // >运算符重载
    //d1 > d2，直接比较他们的年月日的大小即可
    bool operator>(const Date &d)
    {
      if(_year > d._year)
      {
        return true;
      }
      else if(_year == d._year)
      {
        if(_month > d._month)
        {
          return true;
        }
        else if(_month == d._month)
        {
          if(_day > d._day)
          {
            return true;
          }
        }
      }
      return false;
    }

    // ==运算符重载
    // 直接比较年月日是否相等
    bool operator==(const Date &d)
    {
      if(d._day == _day && d._month == _month && d._year == _year)
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    // >=运算符重载
    inline bool operator>=(const Date &d)
    {
      return *this > d || *this == d;
    }

    // <运算符重载
    bool operator<(const Date &d)
    {
      return !(*this >= d);
    }

    // <=运算符重载
    bool operator<=(const Date &d)
    {
      return !(*this > d);
    }

    // !=运算符重载
    bool operator!=(const Date &d)
    {
      return !(*this == d);
    }

    // 日期-日期 返回天数
    //d1 - d2
    /*
     * 分两种情况
     * 1. d1 > d2 ，需用d1 - d2
     * 2. d2 > d1 ，需用d2 - d1
     * 但是本质上都是d1 - d2，所以返回都要返回该值（带正负号即可）
     */
    int operator-(const Date &d)
    {
      //设置符号为正
      int flag = 1;
      Date max(*this);
      Date min(d);
      //首先比较大小
      if(*this < d)
      {
        max = d;
        min = *this;
        //重新设置符号为负
        flag = -1;
      }
      int day = 0;
      //注意不能用max - min，这样就无限递归了
      while (min < max)
      {
        ++(min);
        ++day;
      }

      return day * flag;

    }

  private:
    int _year;
    int _month;
    int _day;
};
