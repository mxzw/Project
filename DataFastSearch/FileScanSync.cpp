#include "FileScanSync.h"
#include "Sysutil.h"

//�����Ǿ�̬�ģ�����Ӧ����ǰ���롣
ScanManager ScanManager::scan_;

ScanManager::ScanManager()
{}

//�������߳�
ScanManager& ScanManager::CreateScanManage(const string& path)
{
	//����ɨ���߳�
	thread scan_th(&StartScan, &scan_,path);
	scan_th.detach();

	//��������߳�
	thread watch_th(&StartWatch, &scan_, path);
	watch_th.detach();

	return scan_;
}

//ɨ���߳���ں���
void ScanManager::StartScan(const string& path)
{
	while (1)
	{
		//ʵ�ֻ����ͬ��
		unique_lock<mutex> lock(mutex_);
		cond_.wait(lock);
		ScanFileToSync(path);
	}
}

//�����������ļ��µ�ȫ�ֱ�������������Ӧ�ý��䵼�����
extern unsigned long g_file_count;
extern unsigned long g_scan_count;

bool ScanManager::DirectionWatch(const string& path)
{
	//����ɨ���ȡ��ǰĿ¼�µ��ļ�����
	GetFileCount(path);

	//�����ͨ���Ƚ�ɨ��ĸ������ļ��ĸ������ж��Ƿ���Ҫɨ��
	return g_scan_count != g_file_count;
}


void ScanManager::StartWatch(const string& path)
{
	while (1)
	{
		bool isChange = DirectionWatch(path);
		if (isChange)
		{
			//�����ص��ļ��������ͱ仯������µ�ǰ�ļ���������֪ͨɨ���߳̽���ɨ��
			g_file_count = g_scan_count;
			cond_.notify_one();
		}
	}
}


void ScanManager::ScanFileToSync(const string& path)
{
	//ɨ�豾���ļ���������ؼ�����
	vector<string> local_sub_files;
	vector<string> local_sub_dirs;
	FileList(path, local_sub_files, local_sub_dirs);

	//һ��Ҫ�����������е��ļ�ȫ�����룬���ڽ���ͬ��
	multiset<string> local_set;
	local_set.insert(local_sub_files.begin(), local_sub_files.end());
	local_set.insert(local_sub_dirs.begin(), local_sub_dirs.end());

	//��ȡ���ݿ��н��
	multiset<string> dataBase_set;
	DataManager& dm_ = DataManager::CreateDataManager();
	dm_.GetFiles(path, dataBase_set);

	//����ͬ��
	// ͬ����ԭ���Ƕ�����multiset���ϵĵ��������жԱȣ���Ϊmultiset�ĵײ��Ǻ������
	// �����ڶԱȵ�ʱ��ֻ�������������жԱȼ��ɡ�
	// �����local < dataBase��ʱ��˵�������������ļ����������ݿ���ִ�и��µĲ���
	// ����� local > dataBase��ʱ��˵������ɾ���˵�ǰ���ݿ���ָ�ļ�������������ݿ���ִ��ɾ���Ĳ���
	// ��� local == dataBase��˵�����غ����ݿ��и��ļ������ڣ�����ƶ����������ɡ�
	
	multiset<string>::iterator local_it = local_set.begin();
	multiset<string>::iterator dataBase_it = dataBase_set.begin();

	while (local_it != local_set.end() && dataBase_it != dataBase_set.end())
	{
		if (*local_it < *dataBase_it)
		{
			dm_.InsertFile(path, *local_it);
			++local_it;
		}
		else if (*local_it > *dataBase_it)
		{
			dm_.DeleteFile(path, *dataBase_it);
			++dataBase_it;
		}
		else
		{
			++local_it;
			++dataBase_it;
		}
	}

	//���ܴ������ݿ��ȱ����꣬��ʣ�µı����ļ����뼴��
	while (local_it != local_set.end())
	{
		dm_.InsertFile(path, *local_it);
		++local_it;
	}

	//���ܴ��ڱ����ļ��ȱ����꣬��ʣ�µ����ݿ��ļ�ɾ������
	while (dataBase_it != dataBase_set.end())
	{
		dm_.DeleteFile(path, *dataBase_it);
		++dataBase_it;
	}
	

	//Ȼ��ʼɨ����Ŀ¼
	//ɨ����Ŀ¼ֻ���ȡlocal_sub_dirs�еĽ�����ݹ���ü��ɡ�
	for (const auto& e : local_sub_dirs)
	{
		string dir_path = path;
		dir_path += "\\";
		dir_path += e;
		ScanFileToSync(dir_path);
	}
}


