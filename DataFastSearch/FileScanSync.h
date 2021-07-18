#pragma once

#include "DataManager.h"

//如果在多线程的情况下运行该程序，则会导致对此类对象被创建多次，但是此类对象的功能是单一的，
// 因此多次创建只会导致资源浪费，因此，这里采用单例模式来对此类进行管理
class ScanManager {
public:
	static ScanManager& CreateScanManage(const string& path);
public:
	void StartScan(const string& path);
	void StartWatch(const string& path);
	bool DirectionWatch(const string& path);
public:
	void ScanFileToSync(const string& path);

private:
	ScanManager();
	~ScanManager() {}

private:
	//DataManager dm_;
	
	static ScanManager scan_;
	mutex mutex_; //锁变量
	condition_variable cond_;//条件变量
};

