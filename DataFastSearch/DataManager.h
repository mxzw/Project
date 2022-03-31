#include "Common.h"
#include "SqliteManager.h"


#define File_DataBase "file.db"
#define File_Table "file_table"
#define SQL_BUFFER_SIZE 1024


//ͬ���������Ҳ��FileScanSyncһ����Ҫʵ�ֵ�����
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

	//�ַ����ָ�������ݴ�����ַ������Լ����ַ�������Ҫ�������ַ�����
	// �����ַ����ָ�Ϊǰ׺����������׺�Ĳ���
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

//Ϊ�˲��������Ľ��б������ͷţ���������RAII�����Զ���ȡ�������ͷű�ռ�
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
