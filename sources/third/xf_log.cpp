
#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <iostream>

#include "PathUtils.h"
#include "sc_log.h"

sc_log sc_log::_global_log;

// 生成日志扩展信息
template<unsigned int n>
inline bool _FormatLog(char(&buf)[n])
{
    time_t tnow = 0; tm t;
    if ((tnow = time(nullptr)) < 0)
        return false;
    if (localtime_s(&t, &tnow) != 0)
        return false;

    return (0 < sprintf_s(buf, "%02d-%02d %02d:%02d:%02d.%03u", t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, clock() % 1000));
}

sc_log::sc_log(const char * appPath)
{
    if (appPath != nullptr)
    {
        strcpy_s(_log_file, AppointSuffix(appPath, ".log").c_str());
    }
}

bool sc_log::operator()(const char * format, ...)
{
    char extra[32]{ 0 };
    if (_FormatLog(extra))
    {
        va_list args;
        va_start(args, format);

        char strText[_MaxLog]{ 0 };

// 根据宏定义确定日志的输出位置
#if __SC_LOG_ == 0
        // 格式化参数并输出到控制台
        bool result = (0 <= vsnprintf(strText, _MaxLog, format, args) && _output_to_console(extra, strText));
#else
        // 格式化参数并输出到文件
        bool result = (0 <= vsnprintf(strText, _MaxLog, format, args) && _output_to_file(extra, strText));
#endif  // __SC_LOG_

        va_end(args);
        return result;
    }

    return false;
}

bool sc_log::InitLog(const char * appPath)
{
    if (appPath != nullptr)
    {
        strcpy_s(_global_log._log_file, AppointSuffix(appPath, ".log").c_str());
        return true;
    }

    return false;
}

std::string sc_log::_MakeText(const std::vector<std::string>& vtr)
{
    char buf[_MaxLog]{ 0 };
    unsigned int index = 0;
    for (const std::string& str : vtr)
    {
        int n = sprintf_s(buf + index, _MaxLog - index, " %s", str.c_str());
        if (n < 0)
            break;

        index += n;
        if (_MaxLog <= index)
            break;
    }

    return { buf };
}

bool sc_log::_output_to_console(const char * extra, const char * text)
{
    _log_mutex.lock();
    std::cout << extra << "[" << std::this_thread::get_id() << "]-> " << text << std::endl;
    _log_mutex.unlock();

    return true;
}

bool sc_log::_output_to_file(const char * extra, const char * text)
{
    std::ofstream fout;
    fout.open(_log_file, std::ios::out | std::ios::app);

    if (fout.is_open())
    {
        _log_mutex.lock();
        fout << extra << "[" << std::this_thread::get_id() << "]-> " << text << std::endl;
        _log_mutex.unlock();

        fout.close();

        return true;
    }

    return false;
}
