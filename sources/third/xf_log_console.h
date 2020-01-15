/*
A simple console log library.
version 1.0.0
https://github.com/xf-bnb/ConsoleLog

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX - License - Identifier : MIT
Copyright(c) 2020 Frank Xiong <https://github.com/xf-bnb>.
*/

#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>

namespace xf::log
{
    inline const char* version() { return "1.0.0"; }

    // 单条日志最大长度
    inline constexpr unsigned int _max_length() { return 0x0400; }

    // 日志全局锁
    inline std::mutex& _log_mutex() { static std::mutex _inst_; return (_inst_); }

    // 输出日志
    inline void _output_to_console(const char* text)
    {
        auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        auto tt = std::chrono::system_clock::to_time_t(tp);

        struct tm lt;
#ifdef _MSC_VER
        localtime_s(&lt, &tt);
#else
        localtime_r(&tt, &lt);
#endif

        char info[32]{ 0 };
        std::snprintf(info, 32, "%02d:%02d:%02d.%03d", lt.tm_hour, lt.tm_min, lt.tm_sec, int(tp.time_since_epoch().count() % 1000));

        std::lock_guard<std::mutex> _auto_lock(_log_mutex());
        std::cout << info << "(" << std::this_thread::get_id() << ")-> " << text << std::endl;
    }

}   // namespace xf::log

#ifdef  _xf_log_console
#define _xfLog(...)  { char _xf_console_log_text_variable[xf::log::_max_length()]{ 0 };     \
        std::snprintf(_xf_console_log_text_variable, xf::log::_max_length(), __VA_ARGS__);  \
        xf::log::_output_to_console(_xf_console_log_text_variable); }
#else
#define _xfLog(...)
#endif  // _xf_log_console
