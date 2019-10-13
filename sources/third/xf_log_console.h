#pragma once

#include <iostream>
#include <sstream>
#include <cstdarg>
#include <mutex>
#include <chrono>
#include <ctime>

namespace xf::log
{
    class Log final
    {
    private:

        mutable std::mutex _log_mutex;

        Log() = default;
        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

        static inline std::ostream& _output_hint()
        {
            auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            auto tt = std::chrono::system_clock::to_time_t(tp);
            struct tm lt;
            localtime_s(&lt, &tt);

            // auto lt = std::localtime(&tt);

            char text[32]{ 0 };
            std::snprintf(text, 32, "%02d:%02d:%02d.%03d", lt.tm_hour, lt.tm_min, lt.tm_sec, int(tp.time_since_epoch().count() % 1000));
            
            return std::cout << text << "(" << std::this_thread::get_id() << ")-> ";
        }

        inline void _output(const char* text) const
        {
            std::lock_guard<std::mutex> _auto_lock(_log_mutex);

            _output_hint() << text << std::endl;
        }

    public:

        static constexpr unsigned int max_length{ 0x0400 };     // 单条日志最大字符数

        void operator()(const char* format, ...) const
        {
            char strText[max_length]{ 0 };

            va_list args;
            va_start(args, format);
            std::vsnprintf(strText, max_length, format, args);
            va_end(args);

            _output(strText);
        }

        static Log& GetInstance() { static Log _log; return (_log); }

    };  // class Log

    template<typename _IterType>
    std::string to_string(_IterType first, _IterType last)
    {
        std::ostringstream oss;
        oss << '[';
        if (first != last) oss << *first++;
        while (first != last) oss << ", " << *first++;
        oss << ']';

        return oss.str();
    }

    template<typename _Type>
    std::string to_string(const _Type& value)
    {
        return xf::log::to_string(std::cbegin(value), std::cend(value));
    }

    template<typename _Type, std::size_t n>
    std::string to_string(const _Type(&value)[n])
    {
        return xf::log::to_string(value, value + n);
    }

}   // namespace xf::log

#ifdef  _xf_log_console
#define _xflog  xf::log::Log::GetInstance()
#else
#define _xflog 
#endif  // _xf_log_console
