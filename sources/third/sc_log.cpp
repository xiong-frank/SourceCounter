
#include <cstdio>
#include <cstdarg>
#include <fstream>

// #include "PathUtils.h"
#include "th.h"

sc_log sc_log::_global_log;

namespace xf::log
{

    bool sc_log::operator()(const char* format, ...)
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

    std::string sc_log::to_string(const std::vector<std::string>& vtr)
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


}
