#include <iostream>

#include "log/sc_log.h"
#include "counter/FileReport.h"
#include "counter/ReportList.h"

bool ReportList::AddReport(const FileReport& r)
{
    std::lock_guard<std::mutex> automtx(m_Mutex);

    if (m_Index < m_TotalFile)
    {
        m_Reports[m_Index++] = r;

        m_TotalLines        += r.GetTotal();
        m_TotalEmpty        += r.GetEmpty();
        m_TotalEffective    += r.GetEffective();
        m_TotalComment      += r.GetComment();
        m_TotalTime         += r.GetSpendTime();

        return true;
    }

    return false;
}

void ReportList::Show() const
{
    for (unsigned int i = 0; i < m_Index; ++i)
        m_Reports[i].Show();

    _log("TotalFile: %d, TotalLines: %d, TotalEmpty: %d, TotalEffective: %d, TotalComment: %d, TotalTime: %d"
        , m_TotalFile
        , m_TotalLines
        , m_TotalEmpty
        , m_TotalEffective
        , m_TotalComment
        , m_TotalTime);
}
