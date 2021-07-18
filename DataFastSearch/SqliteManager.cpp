#include "SqliteManager.h"
#pragma comment(lib,"./sqlite3/sqlite3.lib")


SqliteManager::SqliteManager() :con_(nullptr)
{}
SqliteManager::~SqliteManager()
{
	Close();
}

//根据路径打开Sqlite3数据库
void SqliteManager::Open(const string &path)
{
	int rt = sqlite3_open(path.c_str(), &con_);
	if(rt != SQLITE_OK)
	{
		//fprintf(stderr, "DateBase Open Failed error: %s\n", sqlite3_errmsg(con_));
		ERROR_LOG("DateBase Open Failed error: %s\n", sqlite3_errmsg(con_));
		exit(0);
	}
	else
	{
		//fprintf(stderr, "DateBase Open Successfully\n");
		TRACE_LOG("DateBase Open Successfully\n");
	}


}

//执行相应数据库语句(增删改查)
void SqliteManager::Exec(const string& sql)
{
	char* zErrorMsg = 0;
	int ret = sqlite3_exec(con_, sql.c_str(), NULL, NULL, &zErrorMsg);
	if(ret != SQLITE_OK)
	{
		//fprintf(stderr, "Exec DateBase Failed error: %s\n", sqlite3_errmsg(con_));
		ERROR_LOG("Exec DateBase Failed error: %s\n", sqlite3_errmsg(con_));
		exit(0);
	}
	else
	{
		//fprintf(stderr, "Exec DateBase Successfully\n");
		TRACE_LOG("Exec DateBase Successfully\n");
	}
}

//关闭数据库的连接
void SqliteManager::Close()
{
	if (con_)
	{
		int rt = sqlite3_close(con_);
		if (rt != SQLITE_OK)
		{
			//fprintf(stderr, "DateBase Close Failed error: %s\n", sqlite3_errmsg(con_));
			ERROR_LOG("DateBase Close Failed error: %s\n", sqlite3_errmsg(con_));
			exit(0);
		}
		else
		{
			//fprintf(stderr, "DateBase Close Successfully\n");
			TRACE_LOG("DateBase Close Successfully\n");
		}
	}
	
}

//将查询到的结果存入在ppRet中
void SqliteManager::GetResultTable(const string& sql, char**& ppRet, int& row, int& col)
{
	char* zErrorMsg = 0;
	int rt = sqlite3_get_table(con_, sql.c_str(), &ppRet, &row, &col, &zErrorMsg);
	if (rt != SQLITE_OK)
	{
		//fprintf(stderr, "Get Result Failed error: %s\n", sqlite3_errmsg(con_));
		ERROR_LOG("Get Result Failed error: %s\n", sqlite3_errmsg(con_));
		exit(0);
	}
	else
	{
		//fprintf(stderr, "Get Result Successfully\n");
		TRACE_LOG("Get Result Successfully\n");
	}

}
