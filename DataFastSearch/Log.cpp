#include "Log.h"

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

inline static void __TraceDeBug(const char* fileName, int line, const char* function, const char* format, ...)
{
#ifdef __TRACE__
	//输出调用函数的信息
	fprintf(stdout, "[TRACE][%s:%d:%s]:", GetFileName(fileName).c_str(),line,function);

	//输出用户打的trace信息
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);

	fprintf(stdout, "\n");
#endif // __TRACE__
}

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

