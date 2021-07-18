#pragma once

#include "Common.h"
#include "Log.hpp"

class SqliteManager
{
public:
	SqliteManager();
	~SqliteManager();
public:
	void Open(const string& path);
	void Exec(const string& sql);
	void Close();
	void GetResultTable(const string& sql,char **& ppRet,int &row,int &col);

private:
	sqlite3* con_;
};