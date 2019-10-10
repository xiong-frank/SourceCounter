#ifndef _SC_Log_H_
#define _SC_Log_H_

#include <mutex>
#include <vector>

// 一个简单的日志类
class sc_log
{
private:

    char _log_file[0x0100]{ 0 };        // 日志文件路径
    std::mutex _log_mutex;              // 日志锁

    // 禁止拷贝
    sc_log(const sc_log&) = delete;
    sc_log& operator=(const sc_log&) = delete;

    // 输出日志到控制台
    bool _output_to_console(const char* extra, const char* text);

    // 输出日志到文件
    bool _output_to_file(const char* extra, const char* text);

    // 全局日志对象
    static sc_log _global_log;

public:

    // 单条日志最大字符数
    constexpr static unsigned int _MaxLog{ 0x0400 };

    sc_log() = default;
    explicit sc_log(const char* appPath);

    const char* GetLogFile() const { return _log_file; }

    // 输出日志操作符
    bool operator()(const char* format, ...);

    // 初始化全局日志对象
    static bool InitLog(const char* appPath);

    // 获取全局日志对象实例
    static sc_log& GetInstance() { return _global_log; }

    static std::string _MakeText(const std::vector<std::string>& vtr);

};

// 日志宏定义
#ifdef __SC_LOG_

#define _log_init(path) sc_log::InitLog(path)
#define _log            sc_log::GetInstance()

#else

#define _log_init 
#define _log

#endif  // __SC_LOG_


#endif  // _SC_Log_H_
