#pragma once

#include "Common.h"

//ɨ�豾���ļ����������multiset�У������ļ������ݿ��ͬ��
void FileList(const string& path, vector<string>& subfile, vector<string>& subdir);

//����תƴ��ȫƴ
string ChineseConvertPinYinAllSpell(const string& dest_chinese);

//����תƴ������ĸ
string ChineseConvertPinYinInitials(const string& name);



size_t GetFileCount(const string& path);

//�ַ�����ӡ����
void ColourPrint(const char* str);
