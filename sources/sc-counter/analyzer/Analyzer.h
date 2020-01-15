/*
 * 定义分析器类
 * 定义行的判定类型和分析状态
 * 提供分析函数返回统计结果
 */

#pragma once

namespace sc
{
    class Analyzer
    {
    public:

        // 定义行类型
        enum line_t
        {
            is_blank    = 0x00,     // 空白行
            has_code    = 0x01,     // 有代码
            has_comment = 0x02      // 有注释
        };

        // 定义行状态
        enum class status_t : unsigned char
        {
            normal,                 // 无状态
            quoting,                // 引用中
            primitive,              // 引用中
            annotating              // 注释中
        };

        enum class _symbol_t : unsigned char {
            // 默认符号顺序：无、单行注释、多行注释、字符串、原生字符串
            _nothing, _st_1, _st_2, _st_3, _st_4
        };

    protected:

        // 当前分析状态
        status_t _status{ status_t::normal };

        using _find_symbol_t = std::size_t(*)(const std::string_view&, const std::string&);

        /*
         * 下面4个函数分别对应不同分析状态下的处理逻辑，通过循环转移状态分析的方式，最终完成对一行代码的类型判定。
         */
        virtual unsigned int _OnNormal(std::string_view& line, pair_t& arg, const syntax_t& item);
        virtual unsigned int _OnQuoting(std::string_view& line, pair_t& arg, const syntax_t& item);
        virtual unsigned int _OnPrimitive(std::string_view& line, pair_t& arg, const syntax_t& item);
        virtual unsigned int _OnAnnotating(std::string_view& line, pair_t& arg, const syntax_t& item);

        // 查找起始符号和位置，返回找到的符号类型。
        virtual _symbol_t _search_begin(std::string_view& line, std::size_t& index, pair_t& arg, const syntax_t& item);
        // 查找结束符号，返回行的判定类型。
        virtual unsigned int _search_end(line_t lt, std::string_view& line, pair_t& arg, const syntax_t& item, _find_symbol_t pf);

        // 查找引号结束符位置，同时检查转义字符。
        static std::size_t _find_quote(const std::string_view& view, const std::string& quote)
        {
            for (std::size_t start = 0;;)
            {
                auto index = view.find(quote, start);
                if (std::string::npos == index)
                    return std::string::npos;

                unsigned int n = 0;
                for (auto i = index; (0 < i) && ('\\' == view[--i]); ++n);

                if (0 == n % 2)
                    return index;

                start = index + 1;
            }
        }

    public:

        Analyzer() = default;
        virtual ~Analyzer() { }

        virtual report_t Analyze(const std::string& file, const syntax_t& item, unsigned int mode);

        static report_t Analyze(const std::string& file, const std::string& type, const syntax_t& item, unsigned int mode);

    };  // class Analyzer

}   // namespace sc
