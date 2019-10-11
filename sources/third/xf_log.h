#ifndef _SC_Log_H_
#define _SC_Log_H_

#include <iostream>
#include <mutex>
#include <vector>
#include <chrono>
#include <ctime>

namespace xf::log
{
    // 一个简单的日志类
    class Log final
    {
    private:

        char _log_file[0x0100]{ 0 };        // 日志文件路径
        std::mutex _log_mutex;              // 日志锁

        Log() = default;
        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

        // 输出日志
        bool _output(const char* extra, const char* text)
        {
            std::lock_guard<std::mutex> _auto_lock(_log_mutex);

            _output_hint();
            std::cout << text << std::endl;

            return true;
        }

        static inline void _output_hint()
        {
            auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            auto tt = std::chrono::system_clock::to_time_t(tp);
            auto lt = std::localtime(&tt);
            auto mills = tp.time_since_epoch().count() % 1000;

            char text[32]{ 0 };
            std::snprintf(text, 32, "%02d:%02d:%02d.%03u", lt->tm_hour, lt->tm_min, lt->tm_sec, mills);
            std::cout << text << "(" << std::this_thread::get_id() << ")-> ";
        }

    public:

        // 单条日志最大字符数
        constexpr static unsigned int _MaxLog{ 0x0400 };


        // 输出日志操作符
        bool operator()(const char* format, ...);


        // 获取全局日志对象实例
        static Log& GetInstance() { static Log _log; return (_log); }

        static std::string to_string(const std::vector<std::string>& vtr);

    };

}

// 日志宏定义
#ifdef __SC_LOG_

#define _log_init(path) sc_log::InitLog(path)
#define _log            sc_log::GetInstance()

#else

#define _log_init 
#define _log

#endif  // __SC_LOG_


#endif  // _SC_Log_H_
