#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "third/xf_log_console.h"

#include "counter/FileReport.h"
#include "counter/Analyzer.h"

namespace sc
{
    ReportItem Analyzer::Analyze(const std::string& file)
    {
        // 定义该文件的统计报告对象
        ReportItem report;
        try
        {
            std::ifstream fin(file);
            if (fin.is_open())
            {
                unsigned int (Analyzer:: * funcs[3])(const std::string&, std::size_t) = { &Analyzer::_OnNothing, &Analyzer::_OnQuoting, &Analyzer::_OnAnnotating };

                std::string line;
                for (_analyze_arg aa; std::getline(fin, line); )
                {
                    unsigned int lt = (this->*funcs[static_cast<unsigned int>(_status)])(line, 0);

                    // _xflog("line: %s\ntype: %d, mode: %d", line, ly, static_cast<int>(aa._lm));

                    // 物理行数+1
                    report.AddLines();

                    /*
                     *  ...
                     */

                    switch (_status)
                    {
                    case LineStatus::Nothing:
                        break;
                    case LineStatus::Quoting:
                        break;
                    case LineStatus::Annotating:
                        break;
                    default:
                        break;
                    }

                    // 根据分析结果统计对应类型的行数
                    if (LineType::HasCode == (LineType::HasCode & lt))
                        report.AddCodes(1);
                    if (LineType::HasComment == (LineType::HasComment & lt))
                        report.AddComments(1);

                    // 如果是带有空行标记并且没有其他类型标记则为空行
                    if (LineType::Blank == (LineType::Blank & lt) && 0 == (lt & (LineType::HasCode | LineType::HasComment)))
                        report.AddBlanks(1);
                }

                fin.close();
            }
            else
            {
                _xflog("open file \"%s\" failed !", file.c_str());
            }
        }
        catch (const std::exception & _ex)
        {
            _xflog("analyze file \"%s\" occur exception: %s", file.c_str(), _ex.what());
        }

        _xflog("file: %s, lines: %d, codes: %d, blanks: %d, comments: %d"
               , file.c_str()
               , report.Lines()
               , report.Codes()
               , report.Comments()
               , report.Blanks());

        return report;
    }

    // 返回有效的最小值索引
    static inline unsigned int _TellFirstPos(const long long(&_arr)[4])
    {
        int minpos = INT_MAX;
        unsigned int x = 0;

        for (unsigned int i = 1; i < 4; ++i)
            if (0 < _arr[i] && _arr[i] < _arr[x])
                x = i;

        return x;
    }

    // 查找第一个有效的引号位置
    static inline const char* _FindQuotePos(const char* start)
    {
        if (*start == '\"')
            return start;

        for (;;)
        {
            // 查找引号字符位置
            const char* ptr = strchr(start, '\"');
            // 找不到直接返回空
            if (ptr == nullptr)
                return nullptr;

            // 统计引号前面连续的反斜杠数量
            unsigned int n = 0;
            for (const char* slash = ptr - 1; ; --slash)
            {
                if (*slash == '\\')
                    ++n;
                else
                    break;

                if (slash == start)
                    break;
            }
            // 如果前面的反斜杠数量可以抵消，则找到第一个有效的引号位置
            if (0 == n % 2)
                return ptr;

            // 递增下次查找位置
            start = ptr + 1;
        }
    }
    /*
    unsigned int Analyzer::AnalyzeByNothing(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
    {
        if (*start == 0)
            return AnalyzeThread::LineType::Blank;

        // 查找引号所在位置
        std::ptrdiff_t qkey = -1;
        const char* qptr = _FindQuotePos(start);
        // 如果引号在行首，则进入引用模式
        if (qptr == start)
        {
            _aa._lm = LineStatus::Quoting;
            return LineType::HasCode | AnalyzeByQuoting(_aa, start + 1, singles, multiples);
        }
        // 如果找到则记录找到的位置
        if (qptr != nullptr)
            qkey = qptr - start;

        // 查找第一个单行注释的位置
        std::pair<int, std::ptrdiff_t> skey{ -1, -1 };
        for (unsigned int i = 0; i < singles.size(); ++i)
        {
            const char* ptr = strstr(start, singles[i].c_str());
            if (ptr != nullptr)
            {
                auto pos = ptr - start;
                if (skey.first < 0 || pos < skey.second)
                {
                    skey.first = i;
                    skey.second = pos;
                }
            }
        }
        // 如果单行注释符号在行首，直接返回注释行
        if (skey.second == 0)
        {
            _aa._lm = LineStatus::Nothing;
            return AnalyzeThread::LineType::HasComment;
        }

        // 查找第一个多行注释的位置
        std::pair<int, std::ptrdiff_t> mkey{ -1, -1 };
        for (unsigned int i = 0; i < multiples.size(); ++i)
        {
            const char* ptr = strstr(start, multiples[i].first.c_str());
            if (ptr != nullptr)
            {
                auto pos = ptr - start;
                if (mkey.first < 0 || pos < mkey.second)
                {
                    mkey.first = i;
                    mkey.second = pos;
                }
            }
        }
        // 如果多行注释符号在行首，则进入注释模式
        if (mkey.second == 0)
        {
            _aa._lm = LineStatus::Annotating;
            _aa._arg = mkey.first;
            return LineType::HasComment | AnalyzeByAnnotating(_aa, start + multiples[mkey.first].first.size(), singles, multiples);
        }

        unsigned int index = _TellFirstPos({ INT_MAX, qkey, skey.second, mkey.second });
        switch (index)
        {
        case 0:
        {
            _aa._lm = LineStatus::Nothing;
            // 如果没有找到任何引号、单行注释、多行注释标记，遍历所有字符，有一个非空白符即为有效代码行，否则为空白行。
            for (const unsigned char* ptr = reinterpret_cast<const unsigned char*>(start); *ptr; ++ptr)
                if (0 == isspace(*ptr))
                    return LineType::HasCode;

            return LineType::Blank;
        }
        case 1:
        {
            // 如果引号标记在最前面，则进入引用模式
            _aa._lm = LineStatus::Quoting;
            return AnalyzeThread::LineType::HasCode | AnalyzeByQuoting(_aa, start + qkey + 1, singles, multiples);
        }
        case 2:
        {
            _aa._lm = LineStatus::Nothing;
            // 如果单行注释标记在最前面，检查注释之前是否有有效代码
            for (int i = 0; i < skey.second; ++i)
                if (0 == isspace((unsigned int)start[i]))
                    return LineType::HasCode | LineType::HasComment;

            return LineType::HasComment;
        }
        case 3:
        {
            _aa._lm = LineStatus::Annotating;
            _aa._arg = mkey.first;
            // 如果多行注释标记在最前面，检查注释之前是否有有效代码
            unsigned int ly = AnalyzeThread::LineType::HasComment;
            for (int i = 0; i < mkey.second; ++i)
            {
                if (0 == isspace((unsigned int)start[i]))
                {
                    ly |= LineType::HasCode;
                    break;
                }
            }
            // 如果引号标记在最前面，则进入引用模式
            return ly | AnalyzeByAnnotating(_aa, start + mkey.second + multiples[mkey.first].first.size(), singles, multiples);
        }
        default:
            break;
        }

        return 0;
    }

    unsigned int Analyzer::AnalyzeByQuoting(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
    {
        if (*start == 0)
            return LineType::HasCode;

        // 查找引号结束标记位置
        const char* ptr = _FindQuotePos(start);
        // 找不到直接返回有效代码行
        if (ptr == nullptr)
            return LineType::HasCode;

        // 如果找到则结束引用模式，进入普通模式判断
        _aa._lm = LineStatus::Nothing;
        return LineType::HasCode | AnalyzeByNothing(_aa, ptr + 1, singles, multiples);
    }

    unsigned int Analyzer::AnalyzeByAnnotating(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
    {
        if (*start == 0)
            return LineType::Blank;

        // 查找对应的多行注释结束标记
        const char* ptr = strstr(start, multiples[_aa._arg].second.c_str());
        // 如果没有找到
        if (ptr == nullptr)
        {
            for (ptr = start; *ptr; ++ptr)
                if (0 == isspace(unsigned int(*ptr)))
                    return LineType::HasComment;

            return LineType::Blank;
        }
        else
        {
            // 如果找到，则结束注释模式，进入普通模式判断
            _aa._lm = LineStatus::Nothing;
            return LineType::HasComment | AnalyzeByNothing(_aa, ptr + multiples[_aa._arg].second.size(), singles, multiples);
        }
    }
    */
    unsigned int Analyzer::_OnNothing(const std::string& line, std::size_t index)
    {
        return 0;
    }

    unsigned int Analyzer::_OnQuoting(const std::string& line, std::size_t index)
    {
        return 0;
    }

    unsigned int Analyzer::_OnAnnotating(const std::string& line, std::size_t index)
    {
        return 0;
    }

}
