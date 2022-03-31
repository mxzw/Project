#include "Common.h"
#include "SqliteManager.h"


#define File_DataBase "file.db"
#define File_Table "file_table"
#define SQL_BUFFER_SIZE 1024


//同理，这里的类也和FileScanSync一样，要实现单例化
class DataManager
{
public:
	static DataManager& CreateDataManager();
public:
	void InitSqlite3();
	void InsertFile(const string& path, const string& fileName);
	void DeleteFile(const string& path, const string& fileName);
	void GetFiles(const string& path, multiset<string>& files);

	void SearchFile(const string& key,vector<pair<string,string>>& file_and_path);

	//字符串分割函数，根据传入的字符串，以及该字符串中需要高亮的字符串，
	// 将该字符串分割为前缀、高亮、后缀的部分
	static void splitHighlight(const string& str, const string& key,
		string& prefix, string& highlight, string& suffix);
private:
	DataManager();
	~DataManager();
private:
	SqliteManager sm_;

	static DataManager dm_;
};

////////////////////////////////////////////////////////////////////////////////////////////

//为了不再主动的进行表结果的释放，我们利用RAII机制自动获取表结果和释放表空间
class AutoGetResTable
{
public:
	AutoGetResTable(SqliteManager *sm, const string& sql, char**& ppRet, int& row, int& col);
	~AutoGetResTable();
public:
	AutoGetResTable(const AutoGetResTable&) = delete;
	AutoGetResTable& operator= (const AutoGetResTable&) = delete;

private:
	SqliteManager* sm_;
	char** ppRet_;
};
