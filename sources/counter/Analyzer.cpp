#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <map>

#include "third/xf_log_console.h"

#include "config/Option.h"
#include "config/LangRules.h"

#include "counter/FileReport.h"
#include "counter/Analyzer.h"

namespace sc
{
    ReportItem Analyzer::Analyze(const std::string& file)
    {
        ReportItem report;
        std::ifstream fin(file);
        if (fin.is_open())
        {
            std::function<unsigned int(std::string_view&, _arg_t&)> _status_funcs[4]{
                [this](std::string_view& line, _arg_t& arg) { return this->_OnNormal(line, arg); },
                [this](std::string_view& line, _arg_t& arg) { return this->_OnQuoting(line, arg); },
                [this](std::string_view& line, _arg_t& arg) { return this->_OnPrimitive(line, arg); },
                [this](std::string_view& line, _arg_t& arg) { return this->_OnAnnotating(line, arg); }
            };

            _arg_t arg;
            for (std::string line; std::getline(fin, line); )
            {
                switch (_status_funcs[static_cast<unsigned int>(_status)](std::string_view(line), arg))
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

    Analyzer::_symbol_t Analyzer::_search_begin(std::string_view& line, std::size_t& index, Analyzer::_arg_t& arg)
    {
        _symbol_t st{ _symbol_t::_nothing };

        _search_st_4(st, line, index, arg);
        _search_st_3(st, line, index, arg);
        _search_st_2(st, line, index, arg);
        _search_st_1(st, line, index, arg);

        if (_symbol_t::_st_1 < st)
            line.remove_prefix(arg.first.size() + index);

        return st;
    }

    void Analyzer::_search_st_1(Analyzer::_symbol_t& st, std::string_view& line, std::size_t& index, Analyzer::_arg_t& arg)
    {
        if (_MatchElement(line, index, std::get<0>(_item), arg))
            st = _symbol_t::_st_1;
    }

    void Analyzer::_search_st_2(Analyzer::_symbol_t& st, std::string_view& line, std::size_t& index, Analyzer::_arg_t& arg)
    {
        if (_MatchElement(line, index, std::get<1>(_item), arg))
            st = _symbol_t::_st_2;
    }

    void Analyzer::_search_st_3(Analyzer::_symbol_t& st, std::string_view& line, std::size_t& index, Analyzer::_arg_t& arg)
    {
        if (_MatchElement(line, index, std::get<2>(_item), arg))
            st = _symbol_t::_st_3;
    }

    void Analyzer::_search_st_4(Analyzer::_symbol_t& st, std::string_view& line, std::size_t& index, Analyzer::_arg_t& arg)
    {
        if (_MatchElement(line, index, std::get<3>(_item), arg))
            st = _symbol_t::_st_4;
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
        _symbol_t st = _search_begin(line, index, arg);    // 找到的最前面的符号类型
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

    Analyzer::_symbol_t RubyAnalyzer::_search_begin(std::string_view& line, std::size_t& index, Analyzer::_arg_t& arg)
    {
        _symbol_t st{ _symbol_t::_nothing };

        _search_st_2(st, line, index, arg);
        _search_st_4(st, line, index, arg);
        _search_st_3(st, line, index, arg);
        _search_st_1(st, line, index, arg);

        if (_symbol_t::_st_1 < st)
            line.remove_prefix(arg.first.size() + index);

        return st;
    }

    unsigned int RubyAnalyzer::_OnAnnotating(std::string_view& line, Analyzer::_arg_t& arg)
    {
        if (0 == line.compare(0, arg.second.size(), arg.second))
        {
            _status = status_t::Normal;
            line.remove_prefix(arg.second.size());
            return line_t::has_comment | _OnNormal(line, arg);
        }

        _remove_space(line);

        return line.empty() ? line_t::is_blank : line_t::has_comment;
    }

    void RubyAnalyzer::_search_st_2(Analyzer::_symbol_t& st, std::string_view& line, std::size_t& index, Analyzer::_arg_t& arg)
    {
        const auto& symbol = std::get<1>(_item).front();
        if (0 == line.compare(0, symbol.first.size(), symbol.first))
        {
            index = 0;
            arg = symbol;
            st = _symbol_t::_st_2;
        }
    }

    ReportItem PythonAnalyzer::Analyze(const std::string& file)
    {
        return ReportItem();
    }

}
