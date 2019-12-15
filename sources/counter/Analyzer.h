#pragma once


namespace sc
{
    class ReportItem;

    class Analyzer
    {
    protected:

        // 定义行状态
        enum class status_t : unsigned char
        {
            Normal,                 // 无状态
            Quoting,                // 引用中
            Primitive,              // 引用中
            Annotating              // 注释中
        };

        // 定义行类型
        enum line_t
        {
            is_blank    = 0x00,     // 空白行
            has_code    = 0x01,     // 有代码
            has_comment = 0x02      // 有注释
        };

        enum class _symbol_t : unsigned char {
            // 默认符号顺序：无、字符串、原生字符串、单行注释、多行注释
            _nothing, _st_1, _st_2, _st_3, _st_4
        };


        const LangRules::item_t& _item;
        status_t _status{ status_t::Normal };
        std::pair<std::string, std::string> _arg;

        unsigned int _OnQuoting(std::string_view& line, bool escape);

        virtual unsigned int _OnNormal(std::string_view& line);
        virtual unsigned int _OnQuoting(std::string_view& line);
        virtual unsigned int _OnPrimitive(std::string_view& line);
        virtual unsigned int _OnAnnotating(std::string_view& line);

    public:

        explicit Analyzer(const LangRules::item_t& item) : _item(item) { }

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

    class RubyAnalyzer : public Analyzer
    {
    public:

        using Analyzer::Analyzer;

        virtual ReportItem Analyze(const std::string& file);

    };  // class RubyAnalyzer

    class PythonAnalyzer : public Analyzer
    {
    public:

        using Analyzer::Analyzer;

        virtual ReportItem Analyze(const std::string& file);

    };  // class PythonAnalyzer

}
