#pragma once


namespace sc
{
    class FileReport;

    class Analyzer
    {
    public:

        // 定义行类型
        enum LineType
        {
            UnknowLine = 0x00,     // 未知类型
            EmptyLine = 0x01,     // 空白行
            EffectiveLine = 0x02,     // 有效行
            CommentLine = 0x04,     // 注释行

            TypeMask = 0x0f      // 类型掩码
        };

        // 定义行状态
        enum class LineMode : unsigned char
        {
            Nothing,                    // 无状态
            Quoting,                    // 引用中
            Annotating                  // 注释中
        };

        // 分析参数对象
        struct _analyze_arg {
            LineMode _lm{ LineMode::Nothing };
            unsigned int _arg{ 0 };
        };

        // 对应模式下的分析方法，下述3个函数将相互配合调用共同分析一行数据的类型。
        /*
        static unsigned int AnalyzeByNothing(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        static unsigned int AnalyzeByQuoting(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        static unsigned int AnalyzeByAnnotating(_analyze_arg& aa, const char* start, const list_type& singles, const pair_list& multiples);
        */
    public:

        FileReport Analyze(const std::string& file) const;



    };

}
