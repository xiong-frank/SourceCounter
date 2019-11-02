#pragma once


namespace sc
{
    class ReportItem;

    class Analyzer
    {
    protected:

        // 定义行类型
        enum line_t
        {
            Blank       = 0x00,     // 空白行
            HasCode     = 0x01,     // 有代码
            HasComment  = 0x02      // 有注释
        };

        // 定义行状态
        enum class status_t : unsigned char
        {
            Normal,                 // 无状态
            Quoting,                // 引用中
            Primitive,              // 引用中
            Annotating              // 注释中
        };

        const LangRules::Item& _item;
        status_t _status{ status_t::Normal };
        std::pair<std::string, std::string> _arg;

        unsigned int _OnQuoting(std::string_view& line, bool escape);

        virtual unsigned int _OnNormal(std::string_view& line);
        virtual unsigned int _OnQuoting(std::string_view& line);
        virtual unsigned int _OnPrimitive(std::string_view& line);
        virtual unsigned int _OnAnnotating(std::string_view& line);

    public:

        explicit Analyzer(const LangRules::Item& item) : _item(item) { }

        virtual ReportItem Analyze(const std::string& file);

    };  // class Analyzer

    class CppAnalyzer : public Analyzer
    {
    protected:

        virtual unsigned int _OnPrimitive(std::string_view& line);

    public:

        using Analyzer::Analyzer;

    };  // class CppAnalyzer

    class ClojureAnalyzer : public Analyzer
    {
    protected:

        virtual unsigned int _OnNormal(std::string_view& line);
        virtual unsigned int _OnQuoting(std::string_view& line);
        virtual unsigned int _OnPrimitive(std::string_view& line);
        virtual unsigned int _OnAnnotating(std::string_view& line);

    public:

        using Analyzer::Analyzer;

        virtual ReportItem Analyze(const std::string& file);

    };  // class ClojureAnalyzer

}
