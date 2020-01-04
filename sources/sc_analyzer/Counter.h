#pragma once

#include <map>
#include <vector>
#include <mutex>

#include "ReportType.h"
#include "Rules.h"

namespace sc
{
    // 分析线程类
    class Counter
    {
    public:

        using file_report_t = std::tuple<std::string, std::string, report_t>;

        Counter() = default;

    private:

        std::mutex m_Mutex;
        std::vector<file_report_t> m_Reports;
        pairs_t m_Files;
        Rules m_Rules;

        // 取出一个文件
        bool _PickFile(pair_t& file);

        // 分析线程函数
        std::vector<file_report_t> _Analyze(unsigned int mode);

    public:

        list_t Files() const;
        list_t Files(const std::string& language) const;
        const std::vector<file_report_t>& Reports() const { return m_Reports; }
        const Rules& Rules() const { return m_Rules; }

        // 加载配置
        bool LoadConfig(const std::string& filename) { return m_Rules.Load(filename); }

        // 加载文件
        unsigned int LoadFile(const std::string& input, const std::string& excludes, list_t& includes, bool allowEmpty);

        // 启动线程
        bool Start(unsigned int nThread, unsigned int mode);

    };  // class Counter

}   // namespace sc
