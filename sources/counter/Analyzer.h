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
            // 默认符号顺序：无、单行注释、多行注释、字符串、原生字符串
            _nothing, _st_1, _st_2, _st_3, _st_4
        };

        using pair_t = std::pair<std::string, std::string>;
        using list_t = LangRules::list_t;
        using pairs_t = LangRules::pairs_t;
        using item_t = LangRules::item_t;

        status_t _status{ status_t::Normal };


        virtual unsigned int _OnNormal(std::string_view& line, pair_t& arg, const item_t& item);
        virtual unsigned int _OnQuoting(std::string_view& line, pair_t& arg, const item_t& item);
        virtual unsigned int _OnPrimitive(std::string_view& line, pair_t& arg, const item_t& item);
        virtual unsigned int _OnAnnotating(std::string_view& line, pair_t& arg, const item_t& item);

        virtual unsigned int _OnQuoting(std::string_view& line, pair_t& arg, const item_t& item, bool escape);
        virtual _symbol_t _search_begin(std::string_view& line, std::size_t& index, pair_t& arg, const item_t& item);

    public:

        Analyzer() = default;
        virtual ~Analyzer() { }

        virtual ReportItem Analyze(const std::string& file, const item_t& item);

        static Analyzer& GetAnalyzer(const std::string& name);

    };  // class Analyzer

    class CppAnalyzer : public Analyzer
    {

    };  // class CppAnalyzer

    class RubyAnalyzer : public Analyzer
    {
    public:

        using Analyzer::Analyzer;

        virtual _symbol_t _search_begin(std::string_view& line, std::size_t& index, pair_t& arg, const item_t& item);

        virtual unsigned int _OnAnnotating(std::string_view& line, pair_t& arg, const item_t& item);

    };  // class RubyAnalyzer

    class PythonAnalyzer : public Analyzer
    {

    };  // class PythonAnalyzer

    class ClojureAnalyzer : public Analyzer
    {

    };  // class ClojureAnalyzer

}
