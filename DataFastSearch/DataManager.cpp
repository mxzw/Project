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


//建立数据库表
void DataManager::InitSqlite3()
{
	char sql[SQL_BUFFER_SIZE] = { 0 };
	sprintf(sql, "create table if not exists %s (id INTEGER PRIMARY KEY autoincrement,file_name text,file_path text,file_allspell text, file_initials text); ",File_Table);

	sm_.Exec(sql);
}

//当本地目录下新增文件时，对数据库进行更新(插入)
void DataManager::InsertFile(const string& path, const string& fileName)
{
	char sql[SQL_BUFFER_SIZE] = { 0 };

	string allspell = ChineseConvertPinYinAllSpell(fileName);
	string initials = ChineseConvertPinYinInitials(fileName);

	sprintf(sql, "insert into %s values(null,'%s','%s','%s','%s')"
				,File_Table,fileName.c_str(), path.c_str(),allspell.c_str(),initials.c_str());


	sm_.Exec(sql);
	
}

//当本地目录删除了某一个文件时，对数据库进行更新(删除)
void DataManager::DeleteFile(const string& path, const string& fileName)
{
	char sql[SQL_BUFFER_SIZE] = { 0 };

	sprintf(sql, "delete from %s where file_path = '%s' and file_name = '%s'"
		, File_Table, path.c_str(), fileName.c_str());

	 sm_.Exec(sql);

	 //如果当删除的文件是一个目录的时候，要注意级联删除
	 memset(sql, '\0', SQL_BUFFER_SIZE);

	 string filePath = path;
	 filePath += "//";
	 filePath += fileName;

	 //模糊删除，即删除该目录底下所有的文件
	 sprintf(sql, "delete from %s where file_path like '%s%%' "
		 , File_Table, filePath.c_str());

	 sm_.Exec(sql);
}

//获取数据库中所有的结果，用来与本地目录进行对比，实现数据库和本地文件的同步
void DataManager::GetFiles(const string& path, multiset<string>& files)
{
	char sql[SQL_BUFFER_SIZE] = { 0 };

	sprintf(sql, "select file_name from %s where file_path = '%s'",
					File_Table,path.c_str());

	char** ppRet = 0;
	int row = 0, col = 0;

	//获取从数据库中所得到的结果
	//sm_.GetResultTable(sql, ppRet, row, col);
	AutoGetResTable art(&sm_,sql,ppRet,row,col);

	//将结果放入multiset中，用来与本地目录进行对比，从而实现数据库和本地文件的同步操作
	// ppRet虽然是一个二级指针，但它不是一个二维数组，它里面存放的数据是按一维数据的顺序来进行存放的。
	// 第0行存放的是表名，数据从第一行开始
	for (int i = 1; i <= row; ++i)
	{
		files.insert(ppRet[i]);
	}

	//释放表的结果
	//sqlite3_free_table(ppRet);

}

//根据传入的关键字，从数据库中获取对应的文件名和文件路径
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
		//注意ppRet中是按照一维数组的方式来存取数据的
		file_and_path.push_back(make_pair(ppRet[i*col],ppRet[i*col+1]));
	}

	//sqlite3_free_table(ppRet);
}


//字符串分割函数，用于实现关键字高亮，配合Sysutil.cpp中的颜色打印函数使用。
void DataManager::splitHighlight(const string& str, const string& key,
	string& prefix, string& highlight, string& suffix)
{
	//首先它分为三种情况：1.在原始的字符串中能找到对应的key值
	//					  2. 能够在全拼中找到对应key值
	//					  3. 能够在首字母中找到对应key值
	
	//我们要做的第一步就是将字符串中的字母的大小写统一(统一为小写字母)。
	string low_str(str);
	string low_key(key);

	//利用transform函数将其全部转为小写
	transform(str.begin(),str.end(), low_str.begin(), tolower);

	transform(key.begin(), key.end(), low_key.begin(), tolower);

	//1.在原始字符串中找寻对应key值
	size_t pos = low_str.find(low_key);
	if(pos != string::npos)
	{
		prefix = low_str.substr(0, pos);
		highlight = low_str.substr(pos, low_key.size());
		suffix = low_str.substr(pos + low_key.size(), string::npos);

		return;
	}

	//2.通过全拼进行搜索
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
			
			//如果是英文字符(ascii可见字符范围为0-127)，则每次向前走一个字符
			if (low_str[str_index] >= 0 && low_str[str_index] <= 127)
			{
				++str_index;
				++py_index;
			}
			else
			{
				//中文字符，一次往前走2个字符
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

	//最后就是首字母搜索了
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
	//走到这说明未能搜索到
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





