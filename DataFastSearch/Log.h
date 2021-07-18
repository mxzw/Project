#pragma once

//为了更好的看到项目运行的情况，即每次没必要都显示一堆的成功信息，我们给出日志模块
#ifndef __TRACE__
#define __TRACE__
#endif // !__TRACE__

#ifndef __DEBUG__
#define __DEBUG__
#endif // !__DEBUG__

string GetFileName(const string& path)
{
	char ch = '/';

#ifdef _WIN32
	ch = '\\';
#endif // _WIN32

	size_t pos = path.rfind(ch);

	if (pos == string::npos)
		return path;
	else
		return path.substr(pos + 1);
}

//用于调试回溯的trace log
inline static void __TraceDeBug(const char* fileName, int line, const char* function, const char* format, ...)
{
#ifdef __TRACE__
	//输出调用函数的信息
	fprintf(stdout, "[TRACE][%s:%d:%s]:", GetFileName(fileName).c_str(), line, function);

	//输出用户打的trace信息
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);

	fprintf(stdout, "\n");
#endif // __TRACE__
}


//用于打印错误信息的debug log
inline static void __ErrorDeBug(const char* fileName, int line, const char* function, const char* format, ...)
{
#ifdef __DEBUG__
	//输出调用函数的信息
	fprintf(stdout, "[ERROR][%s:%d:%s]:", GetFileName(fileName).c_str(), line, function);

	//输出用户打的trace信息
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);

	fprintf(stdout, "  errmsg:%s,errno:%d\n", strerror(errno), errno);

#endif // __DEBUG__
}



#define TRACE_LOG(...)\
	__TraceDeBug(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__);

#define ERROR_LOG(...)\
	__ErrorDeBug(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__);

