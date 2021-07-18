#include "FileScanSync.h"
#include "Sysutil.h"

//由于是静态的，所以应该提前引入。
ScanManager ScanManager::scan_;

ScanManager::ScanManager()
{}

//创建多线程
ScanManager& ScanManager::CreateScanManage(const string& path)
{
	//创建扫描线程
	thread scan_th(&StartScan, &scan_,path);
	scan_th.detach();

	//创建监控线程
	thread watch_th(&StartWatch, &scan_, path);
	watch_th.detach();

	return scan_;
}

//扫描线程入口函数
void ScanManager::StartScan(const string& path)
{
	while (1)
	{
		//实现互斥和同步
		unique_lock<mutex> lock(mutex_);
		cond_.wait(lock);
		ScanFileToSync(path);
	}
}

//由于是其他文件下的全局变量，所以我们应该将其导入进来
extern unsigned long g_file_count;
extern unsigned long g_scan_count;

bool ScanManager::DirectionWatch(const string& path)
{
	//首先扫描获取当前目录下的文件个数
	GetFileCount(path);

	//其次再通过比较扫描的个数和文件的个数来判断是否需要扫描
	return g_scan_count != g_file_count;
}


void ScanManager::StartWatch(const string& path)
{
	while (1)
	{
		bool isChange = DirectionWatch(path);
		if (isChange)
		{
			//如果监控到文件个数发送变化，则更新当前文件个数，再通知扫描线程进行扫描
			g_file_count = g_scan_count;
			cond_.notify_one();
		}
	}
}


void ScanManager::ScanFileToSync(const string& path)
{
	//扫描本地文件并放入多重集合中
	vector<string> local_sub_files;
	vector<string> local_sub_dirs;
	FileList(path, local_sub_files, local_sub_dirs);

	//一定要将两个数组中的文件全部插入，用于进行同步
	multiset<string> local_set;
	local_set.insert(local_sub_files.begin(), local_sub_files.end());
	local_set.insert(local_sub_dirs.begin(), local_sub_dirs.end());

	//获取数据库中结果
	multiset<string> dataBase_set;
	DataManager& dm_ = DataManager::CreateDataManager();
	dm_.GetFiles(path, dataBase_set);

	//进行同步
	// 同步的原理是对两个multiset集合的迭代器进行对比，因为multiset的底层是红黑树，
	// 所以在对比的时候只需对其迭代器进行对比即可。
	// 如果当local < dataBase的时候，说明本地新增了文件，需在数据库中执行更新的操作
	// 如果当 local > dataBase的时候，说明本地删除了当前数据库所指文件，因此需在数据库中执行删除的操作
	// 如果 local == dataBase，说明本地和数据库中该文件均存在，向后移动迭代器即可。
	
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

	//可能存在数据库先遍历完，则将剩下的本地文件插入即可
	while (local_it != local_set.end())
	{
		dm_.InsertFile(path, *local_it);
		++local_it;
	}

	//可能存在本地文件先遍历完，则将剩下的数据库文件删除即可
	while (dataBase_it != dataBase_set.end())
	{
		dm_.DeleteFile(path, *dataBase_it);
		++dataBase_it;
	}
	

	//然后开始扫描子目录
	//扫描子目录只需获取local_sub_dirs中的结果，递归调用即可。
	for (const auto& e : local_sub_dirs)
	{
		string dir_path = path;
		dir_path += "\\";
		dir_path += e;
		ScanFileToSync(dir_path);
	}
}


