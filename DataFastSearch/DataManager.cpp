#include "DataManager.h"
#include "Sysutil.h"

DataManager::DataManager()
{
	sm_.Open(File_DataBase);

	InitSqlite3();
}
DataManager::~DataManager()
{
	sm_.Close();
}

DataManager DataManager::dm_;

DataManager& DataManager::CreateDataManager()
{
	return dm_;
}


//�������ݿ��
void DataManager::InitSqlite3()
{
	char sql[SQL_BUFFER_SIZE] = { 0 };
	sprintf(sql, "create table if not exists %s (id INTEGER PRIMARY KEY autoincrement,file_name text,file_path text,file_allspell text, file_initials text); ",File_Table);

	sm_.Exec(sql);
}

//������Ŀ¼�������ļ�ʱ�������ݿ���и���(����)
void DataManager::InsertFile(const string& path, const string& fileName)
{
	char sql[SQL_BUFFER_SIZE] = { 0 };

	string allspell = ChineseConvertPinYinAllSpell(fileName);
	string initials = ChineseConvertPinYinInitials(fileName);

	sprintf(sql, "insert into %s values(null,'%s','%s','%s','%s')"
				,File_Table,fileName.c_str(), path.c_str(),allspell.c_str(),initials.c_str());


	sm_.Exec(sql);
	
}

//������Ŀ¼ɾ����ĳһ���ļ�ʱ�������ݿ���и���(ɾ��)
void DataManager::DeleteFile(const string& path, const string& fileName)
{
	char sql[SQL_BUFFER_SIZE] = { 0 };

	sprintf(sql, "delete from %s where file_path = '%s' and file_name = '%s'"
		, File_Table, path.c_str(), fileName.c_str());

	 sm_.Exec(sql);

	 //�����ɾ�����ļ���һ��Ŀ¼��ʱ��Ҫע�⼶��ɾ��
	 memset(sql, '\0', SQL_BUFFER_SIZE);

	 string filePath = path;
	 filePath += "//";
	 filePath += fileName;

	 //ģ��ɾ������ɾ����Ŀ¼�������е��ļ�
	 sprintf(sql, "delete from %s where file_path like '%s%%' "
		 , File_Table, filePath.c_str());

	 sm_.Exec(sql);
}

//��ȡ���ݿ������еĽ���������뱾��Ŀ¼���жԱȣ�ʵ�����ݿ�ͱ����ļ���ͬ��
void DataManager::GetFiles(const string& path, multiset<string>& files)
{
	char sql[SQL_BUFFER_SIZE] = { 0 };

	sprintf(sql, "select file_name from %s where file_path = '%s'",
					File_Table,path.c_str());

	char** ppRet = 0;
	int row = 0, col = 0;

	//��ȡ�����ݿ������õ��Ľ��
	//sm_.GetResultTable(sql, ppRet, row, col);
	AutoGetResTable art(&sm_,sql,ppRet,row,col);

	//���������multiset�У������뱾��Ŀ¼���жԱȣ��Ӷ�ʵ�����ݿ�ͱ����ļ���ͬ������
	// ppRet��Ȼ��һ������ָ�룬��������һ����ά���飬�������ŵ������ǰ�һά���ݵ�˳�������д�ŵġ�
	// ��0�д�ŵ��Ǳ��������ݴӵ�һ�п�ʼ
	for (int i = 1; i <= row; ++i)
	{
		files.insert(ppRet[i]);
	}

	//�ͷű�Ľ��
	//sqlite3_free_table(ppRet);

}

//���ݴ���Ĺؼ��֣������ݿ��л�ȡ��Ӧ���ļ������ļ�·��
void DataManager::SearchFile(const string& key, vector<pair<string, string>>& file_and_path)
{
	char sql[SQL_BUFFER_SIZE] = { 0 };
	string allspell = ChineseConvertPinYinAllSpell(key);
	string initial = ChineseConvertPinYinInitials(key);
	//file_allspell text, file_initials text
	sprintf(sql, "select file_name,file_path from %s where file_name like '%%%s%%' or							file_allspell like '%%%s%%' or file_initials like '%%%s%%'"
		, File_Table, key.c_str(), allspell.c_str(),initial.c_str());

	int col = 0, row = 0;

	char** ppRet = 0;
	//sm_.GetResultTable(sql, ppRet, row, col);
	AutoGetResTable art(&sm_, sql, ppRet, row, col);


	for (int i = 1; i <= row; ++i)
	{
		//ע��ppRet���ǰ���һά����ķ�ʽ����ȡ���ݵ�
		file_and_path.push_back(make_pair(ppRet[i*col],ppRet[i*col+1]));
	}

	//sqlite3_free_table(ppRet);
}


//�ַ����ָ��������ʵ�ֹؼ��ָ��������Sysutil.cpp�е���ɫ��ӡ����ʹ�á�
void DataManager::splitHighlight(const string& str, const string& key,
	string& prefix, string& highlight, string& suffix)
{
	//��������Ϊ���������1.��ԭʼ���ַ��������ҵ���Ӧ��keyֵ
	//					  2. �ܹ���ȫƴ���ҵ���Ӧkeyֵ
	//					  3. �ܹ�������ĸ���ҵ���Ӧkeyֵ
	
	//����Ҫ���ĵ�һ�����ǽ��ַ����е���ĸ�Ĵ�Сдͳһ(ͳһΪСд��ĸ)��
	string low_str(str);
	string low_key(key);

	//����transform��������ȫ��תΪСд
	transform(str.begin(),str.end(), low_str.begin(), tolower);

	transform(key.begin(), key.end(), low_key.begin(), tolower);

	//1.��ԭʼ�ַ�������Ѱ��Ӧkeyֵ
	size_t pos = low_str.find(low_key);
	if(pos != string::npos)
	{
		prefix = low_str.substr(0, pos);
		highlight = low_str.substr(pos, low_key.size());
		suffix = low_str.substr(pos + low_key.size(), string::npos);

		return;
	}

	//2.ͨ��ȫƴ��������
	string str_allspell = ChineseConvertPinYinAllSpell(low_str);

	string key_allspell = ChineseConvertPinYinAllSpell(low_key);

	pos = str_allspell.find(key_allspell);
	if (pos != string::npos)
	{
		int str_index = 0;
		int py_index = 0;
		int highlight_index = 0;
		int hightlight_len = 0;

		bool first_set = true;

		while (str_index < low_str.size())
		{
			if (py_index >= pos && first_set)
			{
				highlight_index = str_index;
				first_set = false;
			}

			if (py_index >= pos + key_allspell.size())
			{
				hightlight_len = str_index - highlight_index;
				break;
			}
			
			//�����Ӣ���ַ�(ascii�ɼ��ַ���ΧΪ0-127)����ÿ����ǰ��һ���ַ�
			if (low_str[str_index] >= 0 && low_str[str_index] <= 127)
			{
				++str_index;
				++py_index;
			}
			else
			{
				//�����ַ���һ����ǰ��2���ַ�
				string tmp(low_str, str_index, 2);
				string sub_py = ChineseConvertPinYinAllSpell(tmp);

				str_index += 2;
				py_index += sub_py.size();
			}
		}
		prefix = str.substr(0, highlight_index);
		highlight = str.substr(highlight_index, hightlight_len);
		suffix = str.substr(highlight_index + hightlight_len, string::npos);

		return;
	}

	//����������ĸ������
	string str_initials = ChineseConvertPinYinInitials(low_str);
	string key_initials = ChineseConvertPinYinInitials(low_key);

	pos = str_initials.find(key_initials);
	if (pos != string::npos)
	{
		int str_index = 0;
		int initials_index = 0;
		int highlight_index = 0;
		int highlight_len = 0;

		bool first_set = true;
		while (str_index < low_str.size())
		{
			if (initials_index >= pos && first_set)
			{
				highlight_index = str_index;
				first_set = false;
			}

			if (initials_index >= pos + key_initials.size())
			{
				highlight_len = str_index - highlight_index;
				break;
			}
			if (low_str[str_index] >= 0 && low_str[str_index] <= 127)
			{
				++str_index;
				++initials_index;
			}
			else
			{
				str_index += 2;
				++initials_index;
			}
		}
		prefix = str.substr(0, highlight_index);
		highlight = str.substr(highlight_index, highlight_len);
		suffix = str.substr(highlight_index + highlight_len, string::npos);
		return;
	}
	//�ߵ���˵��δ��������
	prefix = str;
	highlight.clear();
	suffix.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

AutoGetResTable::AutoGetResTable(SqliteManager* sm, const string& sql, char**& ppRet, int& row, int& col) : sm_(sm)
{
	sm_->GetResultTable(sql, ppRet, row, col);
	ppRet_ = ppRet;
}
AutoGetResTable::~AutoGetResTable()
{
	sqlite3_free_table(ppRet_);
}





