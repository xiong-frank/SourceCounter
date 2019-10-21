#pragma once


namespace sc
{
    class ReportItem;

    class Analyzer
    {
    public:

        // 定义行类型
        enum line_t
        {
            Unknow      = 0x00,     // 未知类型
            Blank       = 0x01,     // 空白行
            HasCode     = 0x02,     // 有代码
            HasComment  = 0x04      // 有注释
        };

        // 定义行状态
        enum class status_t : unsigned char
        {
            Normal,                     // 无状态
            Quoting,                    // 引用中
            Annotating                  // 注释中
        };

        // 分析参数对象
        /*
        struct _analyze_arg {
            status_t _lm{ status_t::Normal };
            unsigned int _arg{ 0 };
        };
        */
        // 对应模式下的分析方法，下述3个函数将相互配合调用共同分析一行数据的类型。
        /*
        static unsigned int AnalyzeByNothing(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        static unsigned int AnalyzeByQuoting(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        static unsigned int AnalyzeByAnnotating(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        */

        status_t _status{ status_t::Normal };
        std::pair<std::string, std::string> _symbol;

        unsigned int _OnNothing(const std::string& line, std::size_t index);
        unsigned int _OnQuoting(const std::string& line, std::size_t index);
        unsigned int _OnAnnotating(const std::string& line, std::size_t index);

    public:

        ReportItem Analyze(const std::string& file);

    };

}
