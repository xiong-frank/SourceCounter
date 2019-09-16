#ifndef _Report_List_H_
#define _Report_List_H_

#include <mutex>

// 统计报告列表
class ReportList
{
private:

    FileReport* m_Reports;              // 统计报告列表
    unsigned int m_Index;               // 报告数量

    unsigned int m_TotalLines;          // 总代码行数
    unsigned int m_TotalEmpty;          // 总空白行数
    unsigned int m_TotalEffective;      // 总有效行数
    unsigned int m_TotalComment;        // 总注释行数
    unsigned int m_TotalTime;           // 总花费时间
    const unsigned int m_TotalFile;     // 总文件数量

    std::mutex m_Mutex;

    // 禁止拷贝
    ReportList(const ReportList&) = delete;
    ReportList& operator=(const ReportList&) = delete;

public:

    ReportList(unsigned int n)
        : m_TotalFile(n), m_Index(0), m_TotalLines(0), m_TotalEmpty(0), m_TotalEffective(0), m_TotalComment(0), m_TotalTime(0)
    {
        if (0 < n)
            m_Reports = new FileReport[n];
        else
            m_Reports = nullptr;
    }

    ~ReportList()
    {
        if (nullptr != m_Reports)
            delete[] m_Reports;
    }

    unsigned int GetTotalLines() const { return m_TotalLines; }
    unsigned int GetTotalEmpty() const { return m_TotalEmpty; }
    unsigned int GetTotalEffective() const { return m_TotalEffective; }
    unsigned int GetTotalComment() const { return m_TotalComment; }
    unsigned int GetTotalTime() const { return m_TotalTime; }
    unsigned int GetTotalFile() const { return m_TotalFile; }
    unsigned int GetIndex() const { return m_Index; }

    // 对统计报告排序，需要指定对报告的比较方法
    template<typename _FuncT>
    void Sort(_FuncT pfunc)
    {
        std::lock_guard<std::mutex> automtx(m_Mutex);

        // 一个简单的选择排序算法
        unsigned int n = m_Index;
        for (unsigned int maxpos = 0, _index = 0; 0 < n; )
        {
            maxpos = 0;
            for (_index = 1; _index < n; ++_index)
                if (pfunc(m_Reports[maxpos], m_Reports[_index]))
                    maxpos = _index;

            SwapReport(m_Reports[--n], m_Reports[maxpos]);
        }
    }

    bool AddReport(const FileReport& r);

    void Show() const;

};

#endif // _Report_List_H_
