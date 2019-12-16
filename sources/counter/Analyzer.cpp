#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "third/xf_log_console.h"

#include "config/Option.h"
#include "config/LangRules.h"

#include "counter/FileReport.h"
#include "counter/Analyzer.h"

namespace sc
{
    unsigned int Analyzer::_AnalyzeLine(const std::string& line, Analyzer::_arg_t& arg)
    {
        std::string_view view(line);

        switch (_status)
        {
        case status_t::Normal:
            return _OnNormal(view, arg);
        case status_t::Quoting:
            return _OnQuoting(view, arg);
        case status_t::Primitive:
            return _OnPrimitive(view, arg);
        case status_t::Annotating:
            return _OnAnnotating(view, arg);
        default:
            return 0;
        }
    }

    ReportItem Analyzer::Analyze(const std::string& file)
    {
        ReportItem report;
        std::ifstream fin(file);
        if (fin.is_open())
        {
            _arg_t arg;
            for (std::string line; std::getline(fin, line); )
            {
                switch (_AnalyzeLine(line, arg))
                {
                case line_t::has_code:
                    report.AddCodes();
                    break;
                case line_t::has_comment:
                    report.AddComments();
                    break;
                case line_t::is_blank:
                {
                    switch (_status)
                    {
                    case status_t::Quoting:
                        if (_sc_opt.CheckMode(mode_t::ms_is_code)) report.AddCodes();
                        if (_sc_opt.CheckMode(mode_t::ms_is_blank)) report.AddBlanks();
                        break;
                    case status_t::Annotating:
                        if (_sc_opt.CheckMode(mode_t::mc_is_blank)) report.AddBlanks();
                        if (_sc_opt.CheckMode(mode_t::mc_is_comment)) report.AddComments();
                        break;
                    default:
                        report.AddBlanks();
                        break;
                    }
                    break;
                }
                default:
                    if (_sc_opt.CheckMode(mode_t::cc_is_code)) report.AddCodes();
                    if (_sc_opt.CheckMode(mode_t::cc_is_comment)) report.AddComments();
                    break;
                }

                report.AddLines();
            }

            fin.close();
        }
        else
        {
            _xflog("open file \"%s\" failed !", file.c_str());
        }

        _xflog("file: %s, lines: %d, codes: %d, blanks: %d, comments: %d"
               , file.c_str()
               , report.Lines()
               , report.Codes()
               , report.Comments()
               , report.Blanks());

        return report;
    }

    // 移动到可见字符位置
    inline void _remove_space(std::string_view& view)
    {
        std::size_t i = 0;
        while (i < view.size() && std::isspace(view[i]))
            ++i;

        view.remove_prefix(i);
    }

    // 查找引号结束符位置，同时检查转义字符。
    inline std::size_t _find_quote(const std::string_view& view, const std::string& quote)
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

    // 从line的index位置前面查找str
    inline std::size_t _find_front_position(const std::string_view& line, std::size_t index, const std::string& str)
    {
        if (0 < index)
        {
            auto view = line.substr(0, index + str.size() - 1);
            if (auto pos = view.find(str); pos < index)
                return pos;
        }

        return index;
    }

    template<typename _Type> struct is_pair : public std::false_type { };
    template<typename _KeyT, typename _ValueT> struct is_pair<std::pair<_KeyT, _ValueT>> : public std::true_type { };

    template<typename _Type>
    bool _MatchElement(std::string_view& line, std::size_t& index, const list_type<_Type>& seq, Analyzer::_arg_t& arg) {
        bool found{ false };
        for (const auto& v : seq) {
            if constexpr (is_pair<_Type>::value) {
                if (auto i = _find_front_position(line, index, v.first); i < index) {
                    index = i;
                    arg = v;
                    found = true;
                }
            }
            else {
                if (auto i = _find_front_position(line, index, v); i < index) {
                    index = i;
                    found = true;
                }
            }
        }

        return found;
    }

    Analyzer::_symbol_t Analyzer::_FindBegin(std::string_view& line, std::size_t& index, _arg_t& arg)
    {
        _symbol_t st{ _symbol_t::_nothing };
        const auto& [singles, multiples, quotes, primitives] = _item;

        if (_MatchElement(line, index, primitives, arg))
            st = _symbol_t::_st_4;
        if (_MatchElement(line, index, quotes, arg))
            st = _symbol_t::_st_3;
        if (_MatchElement(line, index, multiples, arg))
            st = _symbol_t::_st_2;
        if (_MatchElement(line, index, singles, arg))
            st = _symbol_t::_st_1;

        if (_symbol_t::_st_1 < st)
            line.remove_prefix(arg.first.size() + index);

        return st;
    }

    unsigned int Analyzer::_OnQuoting(std::string_view& line, _arg_t& arg, bool escape)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::is_blank;

        auto index = (escape ? _find_quote(line, arg.second) : line.find(arg.second));
        if (std::string::npos == index)
            return line_t::has_code;

        _status = status_t::Normal;
        line.remove_prefix(arg.second.size() + index);
        return line_t::has_code | _OnNormal(line, arg);
    }

    unsigned int Analyzer::_OnNormal(std::string_view& line, Analyzer::_arg_t& arg)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::is_blank;

        std::size_t index = line.size();                // 找到的最前面的符号位置
        _symbol_t st = _FindBegin(line, index, arg);    // 找到的最前面的符号类型
        line_t lt = line_t::is_blank;

        if (0 < index) lt = line_t::has_code;

        switch (st)
        {
        case _symbol_t::_st_4:
            _status = status_t::Primitive;
            return (lt | _OnPrimitive(line, arg));
        case _symbol_t::_st_3:
            _status = status_t::Quoting;
            return (lt | _OnQuoting(line, arg));
        case _symbol_t::_st_2:
            _status = status_t::Annotating;
            return (lt | line_t::has_comment | _OnAnnotating(line, arg));
        case _symbol_t::_st_1:
            return (lt | line_t::has_comment);
        default:
            return lt;
        }
    }

    unsigned int Analyzer::_OnQuoting(std::string_view& line, _arg_t& arg)
    {
        return _OnQuoting(line, arg, true);
    }

    unsigned int Analyzer::_OnPrimitive(std::string_view& line, _arg_t& arg)
    {
        return _OnQuoting(line, arg, false);
    }

    unsigned int Analyzer::_OnAnnotating(std::string_view& line, _arg_t& arg)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::is_blank;

        auto index = line.find(arg.second);
        if (std::string::npos == index)
            return line_t::has_comment;

        _status = status_t::Normal;
        line.remove_prefix(arg.second.size() + index);
        return line_t::has_comment | _OnNormal(line, arg);
    }

    unsigned int CppAnalyzer::_OnPrimitive(std::string_view& line)
    {
        return 0;
    }

    ReportItem ClojureAnalyzer::Analyze(const std::string& file)
    {
        return ReportItem();
    }

    unsigned int ClojureAnalyzer::_OnNormal(std::string_view& line)
    {
        return 0;
    }

    unsigned int ClojureAnalyzer::_OnQuoting(std::string_view& line)
    {
        return 0;
    }

    unsigned int ClojureAnalyzer::_OnPrimitive(std::string_view& line)
    {
        return 0;
    }

    unsigned int ClojureAnalyzer::_OnAnnotating(std::string_view& line)
    {
        return 0;
    }

    ReportItem RubyAnalyzer::Analyze(const std::string& file)
    {
        return ReportItem();
    }

    ReportItem PythonAnalyzer::Analyze(const std::string& file)
    {
        return ReportItem();
    }

}
