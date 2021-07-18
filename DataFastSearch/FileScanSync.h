#pragma once

#include "DataManager.h"

//����ڶ��̵߳���������иó�����ᵼ�¶Դ�����󱻴�����Σ����Ǵ������Ĺ����ǵ�һ�ģ�
// ��˶�δ���ֻ�ᵼ����Դ�˷ѣ���ˣ�������õ���ģʽ���Դ�����й���
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
	mutex mutex_; //������
	condition_variable cond_;//��������
};

