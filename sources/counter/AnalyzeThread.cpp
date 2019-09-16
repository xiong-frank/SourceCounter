#include <list>
#include <queue>
#include <thread>
#include <iostream>
#include <fstream>

#include "utils/PathUtils.h"
#include "log/sc_log.h"
#include "counter/FileReport.h"
#include "counter/ReportList.h"
#include "counter/AnalyzeThread.h"

bool AnalyzeThread::Start(unsigned int nThread, ReportList& reports, const list_type& singles, const pair_list& multiples)
{
    if (0 < nThread && !m_FileQueue.empty())
    {
        /*
         * 构造 nThread 个线程，将 Analyze 成员函数绑定到线程对象加入到线程容器中。
         * Analyze 方法将在多个线程中同时执行，并将分析结果添加到 reports 参数中。
         * 使用 std::bind 将成员函数 Analyze 与线程对象绑定，使用 std::ref 将参数按引用的方式与线程绑定。
         */
        std::vector<std::thread> vtrThread;
        for (unsigned int i = 0; i < nThread; ++i)
            // vtrThread.emplace_back(std::thread(std::bind(&AnalyzeThread::_Analyze, this, std::ref(reports), std::ref(singles), std::ref(multiples))));
            vtrThread.emplace_back(std::thread([this, &reports, &singles, &multiples]() mutable { this->_Analyze(reports, singles, multiples); }));

        // 依次调用 join 函数等待所有线程执行完成
        for (std::thread& t : vtrThread)
            t.join();

        return true;
    }

    return false;
}

unsigned int AnalyzeThread::ExtractFile(const std::string & fromPath, const list_type & suffixes)
{
    return FilterFile(m_FileQueue, fromPath, suffixes);
}

bool AnalyzeThread::_PickFile(std::string & file)
{
    /*
     * 取文件操作是互斥操作
     * 同一时刻只能有一个线程可以从文件队列取到文件
     * 如果队列为空则取文件失败，否则将取到的文件从参数带出，并将该文件从队列移除。
     */
    std::lock_guard<std::mutex> automtx(m_Mutex);

    if (m_FileQueue.empty())
        return false;

    file = m_FileQueue.front();
    m_FileQueue.pop();
    return true;
}

void AnalyzeThread::_Analyze(ReportList& reports, const list_type& singles, const pair_list& multiples)
{
    // 循环取文件
    for (std::string file; _PickFile(file); )
    {
        // 将分析结果添加到统计报告列表
        reports.AddReport(AnalyzeFile(file, singles, multiples));
    }
}

FileReport AnalyzeThread::AnalyzeFile(const std::string & file, const list_type & singles, const pair_list & multiples)
{
    // 定义该文件的统计报告对象
    FileReport fr(file);
    clock_t t = clock();
    try
    {
        std::ifstream fin;
        fin.open(file);
        if (fin.is_open())
        {
            for (_analyze_arg aa; ; )
            {
                char line[0x0400]{ 0 };
                if (!fin.getline(line, 0x0400))
                    break;

                unsigned int ly = LineType::UnknowLine;
                // 根据当前行的状态分别进行分析
                switch (aa._lm)
                {
                case LineMode::Nothing:
                    ly = AnalyzeByNothing(aa, line, singles, multiples);
                    break;
                case LineMode::Quoting:
                    ly = AnalyzeByQuoting(aa, line, singles, multiples);
                    break;
                case LineMode::Annotating:
                    ly = AnalyzeByAnnotating(aa, line, singles, multiples);
                    break;
                default:
                    break;
                }

                // _log("line: %s\ntype: %d, mode: %d", line, ly, static_cast<int>(aa._lm));

                // 物理行数+1
                fr.AddTotal(1);

                // 根据分析结果统计对应类型的行数
                if (LineType::EffectiveLine == (LineType::EffectiveLine & ly))
                    fr.AddEffective(1);
                if (LineType::CommentLine == (LineType::CommentLine & ly))
                    fr.AddComment(1);

                // 如果是带有空行标记并且没有其他类型标记则为空行
                if (LineType::EmptyLine == (LineType::EmptyLine & ly) && 0 == (ly & (LineType::EffectiveLine | LineType::CommentLine)))
                    fr.AddEmpty(1);
            }

            fin.close();
        }
        else
        {
            _log("open file \"%s\" failed !", file.c_str());
        }
    }
    catch (const std::exception& _ex)
    {
        _log("analyze file \"%s\" occur exception: %s", file.c_str(), _ex.what());
    }

    // 记录分析时间
    fr.SetSpendTime(clock() - t);

    _log("file: %s, tolal: %d, empty: %d, effective: %d, comment: %d, time: %d"
        , fr.GetFilePath().c_str()
        , fr.GetTotal()
        , fr.GetEmpty()
        , fr.GetEffective()
        , fr.GetComment()
        , fr.GetSpendTime());

    return fr;
}

// 返回有效的最小值索引
static inline unsigned int _TellFirstPos(const int(&_arr)[4])
{
    int minpos = INT_MAX;
    unsigned int x = 0;

    for (unsigned int i = 1; i < 4; ++i)
    {
        if (0 < _arr[i] && _arr[i] < minpos)
        {
            x = i;
            minpos = _arr[i];
        }
    }

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

unsigned int AnalyzeThread::AnalyzeByNothing(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
{
    if (*start == 0)
        return AnalyzeThread::LineType::EmptyLine;

    // 查找引号所在位置
    int qkey = -1;
    const char* qptr = _FindQuotePos(start);
    // 如果引号在行首，则进入引用模式
    if (qptr == start)
    {
        _aa._lm = LineMode::Quoting;
        return LineType::EffectiveLine | AnalyzeByQuoting(_aa, start + 1, singles, multiples);
    }
    // 如果找到则记录找到的位置
    if (qptr != nullptr)
        qkey = qptr - start;

    // 查找第一个单行注释的位置
    std::pair<int, int> skey{ -1, -1 };
    for (unsigned int i = 0; i < singles.size(); ++i)
    {
        const char* ptr = strstr(start, singles[i].c_str());
        if (ptr != nullptr)
        {
            int pos = ptr - start;
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
        _aa._lm = LineMode::Nothing;
        return AnalyzeThread::LineType::CommentLine;
    }

    // 查找第一个多行注释的位置
    std::pair<int, int> mkey{ -1, -1 };
    for (unsigned int i = 0; i < multiples.size(); ++i)
    {
        const char* ptr = strstr(start, multiples[i].first.c_str());
        if (ptr != nullptr)
        {
            int pos = ptr - start;
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
        _aa._lm = LineMode::Annotating;
        _aa._arg = mkey.first;
        return LineType::CommentLine | AnalyzeByAnnotating(_aa, start + multiples[mkey.first].first.size(), singles, multiples);
    }

    unsigned int index = _TellFirstPos({ INT_MAX, qkey, skey.second, mkey.second });
    switch (index)
    {
    case 0:
    {
        _aa._lm = LineMode::Nothing;
        // 如果没有找到任何引号、单行注释、多行注释标记，遍历所有字符，有一个非空白符即为有效代码行，否则为空白行。
        for (const unsigned char* ptr = reinterpret_cast<const unsigned char*>(start); *ptr; ++ptr)
            if (0 == isspace(*ptr))
                return LineType::EffectiveLine;

        return LineType::EmptyLine;
    }
    case 1:
    {
        // 如果引号标记在最前面，则进入引用模式
        _aa._lm = LineMode::Quoting;
        return AnalyzeThread::LineType::EffectiveLine | AnalyzeByQuoting(_aa, start + qkey + 1, singles, multiples);
    }
    case 2:
    {
        _aa._lm = LineMode::Nothing;
        // 如果单行注释标记在最前面，检查注释之前是否有有效代码
        for (int i = 0; i < skey.second; ++i)
            if (0 == isspace((unsigned int)start[i]))
                return LineType::EffectiveLine | LineType::CommentLine;

        return LineType::CommentLine;
    }
    case 3:
    {
        _aa._lm = LineMode::Annotating;
        _aa._arg = mkey.first;
        // 如果多行注释标记在最前面，检查注释之前是否有有效代码
        unsigned int ly = AnalyzeThread::LineType::CommentLine;
        for (int i = 0; i < mkey.second; ++i)
        {
            if (0 == isspace((unsigned int)start[i]))
            {
                ly |= LineType::EffectiveLine;
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

unsigned int AnalyzeThread::AnalyzeByQuoting(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
{
    if (*start == 0)
        return LineType::EffectiveLine;

    // 查找引号结束标记位置
    const char* ptr = _FindQuotePos(start);
    // 找不到直接返回有效代码行
    if (ptr == nullptr)
        return LineType::EffectiveLine;

    // 如果找到则结束引用模式，进入普通模式判断
    _aa._lm = LineMode::Nothing;
    return LineType::EffectiveLine | AnalyzeByNothing(_aa, ptr + 1, singles, multiples);
}

unsigned int AnalyzeThread::AnalyzeByAnnotating(_analyze_arg& _aa, const char* start, const list_type& singles, const pair_list& multiples)
{
    if (*start == 0)
        return LineType::EmptyLine;

    // 查找对应的多行注释结束标记
    const char* ptr = strstr(start, multiples[_aa._arg].second.c_str());
    // 如果没有找到
    if (ptr == nullptr)
    {
        for (ptr = start; *ptr; ++ptr)
            if (0 == isspace(unsigned int(*ptr)))
                return LineType::CommentLine;

        return LineType::EmptyLine;
    }
    else
    {
        // 如果找到，则结束注释模式，进入普通模式判断
        _aa._lm = LineMode::Nothing;
        return LineType::CommentLine | AnalyzeByNothing(_aa, ptr + multiples[_aa._arg].second.size(), singles, multiples);
    }
}
