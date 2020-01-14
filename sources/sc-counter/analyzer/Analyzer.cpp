#include <functional>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "../../third/xf_log_console.h"

#include "../ReportType.h"
#include "Analyzer.h"

namespace sc
{
    report_t Analyzer::Analyze(const std::string& file, const syntax_t& item, unsigned int mode)
    {
        report_t report{ 0, 0, 0, 0 };
        auto& [lines, codes, comments, blanks] = report;

        std::ifstream fin(file);
        if (fin.is_open())
        {
            std::function<unsigned int(std::string_view&, pair_t&, const syntax_t&)> _status_funcs[4]{
                [this](std::string_view& line, pair_t& arg, const syntax_t& item) { return this->_OnNormal(line, arg, item); },
                [this](std::string_view& line, pair_t& arg, const syntax_t& item) { return this->_OnQuoting(line, arg, item); },
                [this](std::string_view& line, pair_t& arg, const syntax_t& item) { return this->_OnPrimitive(line, arg, item); },
                [this](std::string_view& line, pair_t& arg, const syntax_t& item) { return this->_OnAnnotating(line, arg, item); }
            };

            pair_t arg;
            for (std::string line; std::getline(fin, line); )
            {
                std::string_view view(line);
                switch (_status_funcs[static_cast<unsigned int>(_status)](view, arg, item))
                {
                case line_t::has_code:
                    ++codes;
                    break;
                case line_t::has_comment:
                    ++comments;
                    break;
                case line_t::is_blank:
                {
                    switch (_status)
                    {
                    case status_t::quoting:
                    case status_t::primitive:
                        if (_check_mode(mode, mode_t::ms_is_code)) ++codes;
                        if (_check_mode(mode, mode_t::ms_is_blank)) ++blanks;
                        break;
                    case status_t::annotating:
                        if (_check_mode(mode, mode_t::mc_is_blank)) ++blanks;
                        if (_check_mode(mode, mode_t::mc_is_comment)) ++comments;
                        break;
                    default:
                        ++blanks;
                        break;
                    }
                    break;
                }
                default:
                {
                    if (_check_mode(mode, mode_t::cc_is_code)) ++codes;
                    if (_check_mode(mode, mode_t::cc_is_comment)) ++comments;
                    break;
                }
                }

                ++lines;
            }

            fin.close();
        }
        else
        {
            _xflog(R"(open file "%s" failed !)", file.c_str());
        }

        _xflog("file: %s, lines: %d, codes: %d, comments: %d, blanks: %d", file.c_str(), lines, codes, comments, blanks);

        return report;
    }

    inline bool _is_space(char c) {
        return (0x20 == c || 0x09 == c || 0x0d == c || 0x0a == c || 0x0c == c || 0x0b == c);
    }

    // 移动到可见字符位置
    inline void _remove_space(std::string_view& view)
    {
        std::size_t i = 0;
        while (i < view.size() && _is_space(view[i]))
            ++i;

        view.remove_prefix(i);
    }

    inline std::size_t _find_quote(const std::string_view& view, const std::string& quote) { return view.find(quote); }

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

    template<Analyzer::_symbol_t _Key, typename _Type>
    void _MatchElement(Analyzer::_symbol_t& st, std::string_view& line, std::size_t& index, const list_type<_Type>& seq, pair_t& arg) {
        for (const auto& v : seq) {
            if constexpr (is_pair<_Type>::value) {
                if (auto i = _find_front_position(line, index, v.first); i < index) {
                    index = i;
                    arg = v;
                    st = _Key;
                }
            } else {
                if (auto i = _find_front_position(line, index, v); i < index) {
                    index = i;
                    st = _Key;
                }
            }
        }
    }

    Analyzer::_symbol_t Analyzer::_search_begin(std::string_view& line, std::size_t& index, pair_t& arg, const syntax_t& item)
    {
        _symbol_t st{ _symbol_t::_nothing };

        _MatchElement<_symbol_t::_st_4>(st, line, index, std::get<3>(item), arg);
        _MatchElement<_symbol_t::_st_3>(st, line, index, std::get<2>(item), arg);
        _MatchElement<_symbol_t::_st_2>(st, line, index, std::get<1>(item), arg);
        _MatchElement<_symbol_t::_st_1>(st, line, index, std::get<0>(item), arg);

        return st;
    }

    unsigned int Analyzer::_OnNormal(std::string_view& line, pair_t& arg, const syntax_t& item)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::is_blank;

        std::size_t index = line.size();                        // 找到的最前面的符号位置
        _symbol_t st = _search_begin(line, index, arg, item);   // 找到的最前面的符号类型

        if (_symbol_t::_st_1 < st)
            line.remove_prefix(arg.first.size() + index);

        line_t lt = line_t::is_blank;
        if (0 < index)
            lt = line_t::has_code;

        switch (st)
        {
        case _symbol_t::_st_4:
            _status = status_t::primitive;
            return (lt | _OnPrimitive(line, arg, item));
        case _symbol_t::_st_3:
            _status = status_t::quoting;
            return (lt | _OnQuoting(line, arg, item));
        case _symbol_t::_st_2:
            _status = status_t::annotating;
            return (lt | line_t::has_comment | _OnAnnotating(line, arg, item));
        case _symbol_t::_st_1:
            return (lt | line_t::has_comment);
        default:
            return lt;
        }
    }

    unsigned int Analyzer::_OnQuoting(std::string_view& line, pair_t& arg, const syntax_t& item)
    {
        return _on_status(line_t::has_code, line, arg, item, sc::Analyzer::_find_quote);
    }

    unsigned int Analyzer::_OnPrimitive(std::string_view& line, pair_t& arg, const syntax_t& item)
    {
        return _on_status(line_t::has_code, line, arg, item, sc::_find_quote);
    }

    unsigned int Analyzer::_OnAnnotating(std::string_view& line, pair_t& arg, const syntax_t& item)
    {
        return _on_status(line_t::has_comment, line, arg, item, sc::_find_quote);
    }

    unsigned int Analyzer::_on_status(line_t lt, std::string_view& line, pair_t& arg, const syntax_t& item, std::size_t (*func)(const std::string_view&, const std::string&))
    {
        _remove_space(line);

        if (line.empty())
            return line_t::is_blank;

        auto index = func(line, arg.second);
        if (std::string::npos == index)
            return (lt);

        _status = status_t::normal;
        line.remove_prefix(arg.second.size() + index);
        return (lt | _OnNormal(line, arg, item));
    }

#include "CppAnalyzer.inl"
#include "RubyAnalyzer.inl"
#include "CsharpAnalyzer.inl"
#include "ClojureAnalyzer.inl"

    template<typename _AnalyzerType>
    report_t _Analyze(const std::string& file, const syntax_t& item, unsigned int mode) {
        return (_AnalyzerType().Analyze(file, item, mode));
    }

    const std::map<string_type, report_t (*)(const std::string&, const syntax_t&, unsigned int), _str_compare> _analyzerMap{
        { "Java",       _Analyze<Analyzer> },
        { "JavaScript", _Analyze<Analyzer> },
        { "C++",        _Analyze<CppAnalyzer> },
        { "C#",         _Analyze<CsharpAnalyzer> },
        { "Clojure",    _Analyze<ClojureAnalyzer> },
        { "Ruby",       _Analyze<RubyAnalyzer> },
        { "Python",     _Analyze<RubyAnalyzer> }
    };

    report_t Analyzer::Analyze(const std::string& file, const std::string& type, const syntax_t& item, unsigned int mode)
    {
        auto iter = _analyzerMap.find(type);
        if (iter != _analyzerMap.end())
            return iter->second(file, item, mode);
        else
            return Analyzer().Analyze(file, item, mode);
    }

}
