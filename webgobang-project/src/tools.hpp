#pragma once 

#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
using namespace std;


class StringTools
{
    public:
        /*
         * 设置为静态主要是为了可以直接通过类名来对其进行调用，而不用通过实例化一个对象来进行调用
         * Split:对传入的字符串按照相应字符进行切割，并将切割完的内容放到数组中
         * 这里我们主要使用的是boost库下的split函数对其进行切割
         *  template< typename SequenceSequenceT, typename RangeT, typename PredicateT >
         inline SequenceSequenceT& split(

             SequenceSequenceT& Result, // 结果集，将最终的切割结果放到result中

 #if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
             RangeT&& Input, //C++11版本  ---需要进行切割的字符串
 #else
             RangeT& Input,
 #endif

             PredicateT Pred,  //切割的标准，----字符/字符串
             token_compress_mode_type eCompress=token_compress_off  // 是否将连续的分隔符看成一个分隔符
             )

             boost::is_any_of(const RangeT &Set)
              它用于生成谓词以验证作为参数传入的字符是否在给定的字符串中存在。
         */ 
        static void Split(const string& input,const string& split_char,vector<string>& sv)
        {
            /*
             * is_any_of : 用于设定切割的分割符，使用它的原因是----不仅仅支持字符切割，也支持字符串切割
             *  因为传入的字符串，有可能表示的是一个字符，也有可能表示一个字符串，所有就需要使用该函数来进行区分
             *
             *  token_compress_off : 将多个分隔符按照多个来计算  abc==a  -->   "abc" "" "a"
             *  token_compress_on : 将多个风格发按照一个来计算   abc==a  -->    "abc" "a"
             */ 
            boost::split(sv,input,boost::is_any_of(split_char),boost::token_compress_off); 
        }

};
