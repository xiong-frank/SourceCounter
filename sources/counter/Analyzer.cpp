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
    ReportItem Analyzer::Analyze(const std::string& file)
    {
        ReportItem report;
        std::ifstream fin(file);
        if (fin.is_open())
        {
            unsigned int (Analyzer::*_funcs[])(std::string_view&) = {
                &Analyzer::_OnNormal,
                &Analyzer::_OnQuoting,
                &Analyzer::_OnPrimitive,
                &Analyzer::_OnAnnotating };

            for (std::string line; std::getline(fin, line); )
            {
                switch ((this->*_funcs[static_cast<unsigned int>(_status)])(std::string_view(line)))
                {
                case line_t::HasCode:
                    report.AddCodes();
                    break;
                case line_t::HasComment:
                    report.AddComments();
                    break;
                case line_t::Blank:
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

    // 移动到可见字符位置
    inline void _remove_space(std::string_view& view)
    {
        for (auto i = 0; i < view.size(); ++i)
        {
            if (0 == std::isspace(view[i]))
            {
                view.remove_prefix(i);
                break;
            }
        }
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

    unsigned int Analyzer::_OnQuoting(std::string_view& line, bool escape)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::Blank;

        auto index = (escape ? _find_quote(line, _arg.second) : line.find(_arg.second));
        if (std::string::npos == index)
            return line_t::HasCode;

        _status = status_t::Normal;
        line.remove_prefix(_arg.second.size() + index);
        return line_t::HasCode | _OnNormal(line);
    }

    unsigned int Analyzer::_OnNormal(std::string_view& line)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::Blank;

        // 符号类型：字符串、原生字符串、单行注释、多行注释
        enum class _first_t : unsigned char { _nothing, _string, _r_str, _slc, _mlc };

        _first_t _ft{ _first_t::_nothing }; // 找到的最前面的符号类型
        std::size_t index = line.size();    // 找到的最前面的符号位置
        const auto& [singles, multiples, quotes, primitives] = _item;

        for (const auto& arg : primitives)
        {
            auto i = _find_front_position(line, index, arg.first);
            if (i < index)
            {
                index = i;
                _ft = _first_t::_r_str;
                _arg = arg;
            }
        }
        for (const auto& arg : quotes)
        {
            auto i = _find_front_position(line, index, arg.first);
            if (i < index)
            {
                index = i;
                _ft = _first_t::_string;
                _arg = arg;
            }
        }
        for (const auto& arg : multiples)
        {
            auto i = _find_front_position(line, index, arg.first);
            if (i < index)
            {
                index = i;
                _ft = _first_t::_mlc;
                _arg = arg;
            }
        }
        for (const auto& arg : singles)
        {
            auto i = _find_front_position(line, index, arg);
            if (i < index)
            {
                index = i;
                _ft = _first_t::_slc;
            }
        }

        line_t lt = line_t::Blank;
        if (0 < index) lt = line_t::HasCode;

        switch (_ft)
        {
        case _first_t::_string:
            _status = status_t::Quoting;
            line.remove_prefix(_arg.first.size() + index);
            return (lt | _OnQuoting(line));
        case _first_t::_r_str:
            _status = status_t::Primitive;
            line.remove_prefix(_arg.first.size() + index);
            return (lt | _OnPrimitive(line));
        case _first_t::_mlc:
            _status = status_t::Annotating;
            line.remove_prefix(_arg.first.size() + index);
            return (lt | line_t::HasComment | _OnAnnotating(line));
        case _first_t::_slc:
            return (lt | line_t::HasComment);
        default:
            return lt;
        }
    }

    unsigned int Analyzer::_OnQuoting(std::string_view& line)
    {
        return _OnQuoting(line, true);
    }

    unsigned int Analyzer::_OnPrimitive(std::string_view& line)
    {
        return _OnQuoting(line, false);
    }

    unsigned int Analyzer::_OnAnnotating(std::string_view& line)
    {
        _remove_space(line);

        if (line.empty())
            return line_t::Blank;

        auto index = line.find(_arg.second);
        if (std::string::npos == index)
            return line_t::HasComment;

        _status = status_t::Normal;
        line.remove_prefix(_arg.second.size() + index);
        return line_t::HasComment | _OnNormal(line);
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
