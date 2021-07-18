#pragma once

#include "Common.h"

//扫描本地文件，并存放在multiset中，用于文件与数据库的同步
void FileList(const string& path, vector<string>& subfile, vector<string>& subdir);

//汉字转拼音全拼
string ChineseConvertPinYinAllSpell(const string& dest_chinese);

//汉字转拼音首字母
string ChineseConvertPinYinInitials(const string& name);



size_t GetFileCount(const string& path);

//字符串打印函数
void ColourPrint(const char* str);
