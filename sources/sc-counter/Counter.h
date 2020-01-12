#pragma once

#include <map>
#include <vector>
#include <atomic>

#include "ReportType.h"
#include "RuleManager.h"

namespace sc
{
    // 分析线程类
    class Counter
    {
    public:

        using file_report_t = std::tuple<std::string, std::string, report_t>;
        using item_t = std::tuple<std::string, const std::string, const std::string, const syntax_t&>;

        Counter() = default;

    private:

        Counter(const Counter&) = delete;
        Counter& operator=(const Counter&) = delete;

        std::vector<file_report_t> m_Reports;
        std::vector<item_t> m_Items;
        RuleManager m_RuleMgr;

        std::atomic_uint m_ItemIndex{ 0 };

        // 分析线程函数
        std::vector<file_report_t> _Analyze(unsigned int mode);

    public:

        list_t Files() const;
        list_t Files(const std::string& language) const;
        const std::vector<file_report_t>& Reports() const { return m_Reports; }
        const RuleManager& RuleMgr() const { return m_RuleMgr; }

        // 加载配置
        bool LoadConfig(const std::string& filename, std::string& error) { return m_RuleMgr.Load(filename, error); }

        // 加载文件
        unsigned int LoadFile(const std::string& input, const std::string& excludes, list_t& includes, bool allowEmpty);

        // 启动线程
        bool Start(unsigned int nThread, unsigned int mode);

    };  // class Counter

}   // namespace sc
