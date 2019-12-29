#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <map>

#include "../../third/xf_log_console.h"

#include "../../config/Option.h"
#include "../../config/LangRules.h"

#include "../../counter/FileReport.h"

#include "Analyzer.h"

namespace sc
{
    ReportItem Analyzer::Analyze(const std::string& file, const Analyzer::item_t& item)
    {
        ReportItem report;

        std::ifstream fin(file);
        if (fin.is_open())
        {
            std::function<unsigned int(std::string_view&, pair_t&, const item_t&)> _status_funcs[4]{
                [this](std::string_view& line, pair_t& arg, const item_t& item) { return this->_OnNormal(line, arg, item); },
                [this](std::string_view& line, pair_t& arg, const item_t& item) { return this->_OnQuoting(line, arg, item); },
                [this](std::string_view& line, pair_t& arg, const item_t& item) { return this->_OnPrimitive(line, arg, item); },
                [this](std::string_view& line, pair_t& arg, const item_t& item) { return this->_OnAnnotating(line, arg, item); }
            };

            pair_t arg;
            for (std::string line; std::getline(fin, line); )
            {
                std::string_view view(line);
                switch (_status_funcs[static_cast<unsigned int>(_status)](view, arg, item))
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
                    case status_t::quoting:
                    case status_t::primitive:
                        if (_sc_opt.CheckMode(mode_t::ms_is_code)) report.AddCodes();
                        if (_sc_opt.CheckMode(mode_t::ms_is_blank)) report.AddBlanks();
                        break;
                    case status_t::annotating:
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
               , file.c_str(), report.Lines(), report.Codes(), report.Comments(), report.Blanks());

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

    template<Analyzer::_symbol_t _Key, typename _Type>
    void _MatchElement(Analyzer::_symbol_t& st, std::string_view& line, std::size_t& index, const list_type<_Type>& seq, std::pair<std::string, std::string>& arg) {
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

    Analyzer::_symbol_t Analyzer::_search_begin(std::string_view& line, std::size_t& index, Analyzer::pair_t& arg, const Analyzer::item_t& item)
    {
        _symbol_t st{ _symbol_t::_nothing };

        _MatchElement<_symbol_t::_st_4>(st, line, index, std::get<3>(item), arg);
        _MatchElement<_symbol_t::_st_3>(st, line, index, std::get<2>(item), arg);
        _MatchElement<_symbol_t::_st_2>(st, line, index, std::get<1>(item), arg);
        _MatchElement<_symbol_t::_st_1>(st, line, index, std::get<0>(item), arg);

        return st;
    }

    unsigned int Analyzer::_OnNormal(std::string_view& line, Analyzer::pair_t& arg, const Analyzer::item_t& item)
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

    unsigned int Analyzer::_OnQuoting(std::string_view& line, Analyzer::pair_t& arg, const Analyzer::item_t& item)
    {
        return _OnQuoting(line, arg, item, true);
    }

    unsigned int Analyzer::_OnPrimitive(std::string_view& line, Analyzer::pair_t& arg, const Analyzer::item_t& item)
    {
        return _OnQuoting(line, arg, item, false);
    }

    unsigned int Analyzer::_OnAnnotating(std::string_view& line, Analyzer::pair_t& arg, const Analyzer::item_t& item)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::is_blank;

        auto index = line.find(arg.second);
        if (std::string::npos == index)
            return line_t::has_comment;

        _status = status_t::normal;
        line.remove_prefix(arg.second.size() + index);
        return line_t::has_comment | _OnNormal(line, arg, item);
    }

    unsigned int Analyzer::_OnQuoting(std::string_view& line, Analyzer::pair_t& arg, const Analyzer::item_t& item, bool escape)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::is_blank;

        auto index = (escape ? _find_quote(line, arg.second) : line.find(arg.second));
        if (std::string::npos == index)
            return line_t::has_code;

        _status = status_t::normal;
        line.remove_prefix(arg.second.size() + index);
        return line_t::has_code | _OnNormal(line, arg, item);
    }

#include "CppAnalyzer.inl"
#include "RubyAnalyzer.inl"
#include "PythonAnalyzer.inl"
#include "ClojureAnalyzer.inl"

    template<typename _AnalyzerType>
    ReportItem _Analyze(const std::string& file, const LangRules::item_t& item) {
        return (_AnalyzerType().Analyze(file, item));
    }

    const std::map<string_type, ReportItem (*)(const std::string&, const LangRules::item_t&), _str_compare> _analyzerMap{
        { "C++",            _Analyze<CppAnalyzer> },
        { "CPlusPlus",      _Analyze<CppAnalyzer> },
        { "Clojure",        _Analyze<ClojureAnalyzer> },
        { "ClojureScript",  _Analyze<ClojureAnalyzer> },
        { "Ruby",           _Analyze<RubyAnalyzer> },
        { "Python",         _Analyze<PythonAnalyzer> }
    };

    ReportItem Analyzer::Analyze(const std::string& file, const std::string& type)
    {
        auto iter = _analyzerMap.find(type);
        if (iter != _analyzerMap.end())
            return iter->second(file, *_sc_lrs.GetRule(type));
        else
            return Analyzer().Analyze(file, *_sc_lrs.GetRule(type));
    }

}
