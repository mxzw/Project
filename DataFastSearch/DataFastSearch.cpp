#include "Sysutil.h"
#include "DataManager.h"
#include "FileScanSync.h"
#include "SysFrame.h"

void test2()
{
	const string path = "D:\\test_projectgo";
	vector<string> v1;
	vector<string> v2;

	FileList(path, v1, v2);

	for (const auto& e : v1)
		cout << e << endl;
	cout << "subdirs ： " << endl;
	for (const auto& e : v2)
		cout << e << " ";
	cout << endl;
}




void Run()
{
	const string path = "D:\\test_project";
	ScanManager &sm = ScanManager::CreateScanManage(path);
	sm.ScanFileToSync(path);

	DataManager &dm = DataManager::CreateDataManager();
	string key;

	vector<pair<string, string>> file_and_path;

	while (1)
	{
		cout << "请输入关键字：";
		cin >> key;

		dm.SearchFile(key, file_and_path);

		printf("%-15s%-50s\n", "名称", "路径");
		for (const auto& e : file_and_path)
		{
			printf("%-15s%-50s\n", e.first.c_str(), e.second.c_str());
		}
		file_and_path.clear();
	}
}

const char* title = "文件海中一束光";

void Main()
{

}

int main()
{
	//Run();
	//Main();
	const string& path = "D:\\test_project";
	ScanManager& sm = ScanManager::CreateScanManage(path);
	sm.ScanFileToSync(path);

	DataManager& dm = DataManager::CreateDataManager();
	string key;

	vector<pair<string, string>> file_and_path;

	while (1)
	{
		DrawFrame((char*)title);
		DrawMenu();

		cin >> key;
		if (key == "exit")
			break;
		dm.SearchFile(key, file_and_path);

		int row = 5;
		int count = 0;

		string prefix, highlight, suffix;
		for (const auto& e : file_and_path)
		{
			string file_name = e.first;
			string file_path = e.second;
			DataManager::splitHighlight(file_name, key, prefix, highlight, suffix);

			SetCurPos(2, row + count++);
			cout << prefix;
			ColourPrint(highlight.c_str());
			cout << suffix;
			SetCurPos(33, row + count - 1);
			printf("%-50s", file_path.c_str());
		}
		file_and_path.clear();
		SystemEnd();
		system("pause");
	}
	SystemEnd();
	return 0;
}