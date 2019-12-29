#pragma once

#include "ReportType.h"
#include "ReportItem.h"
#include "LangRules.h"

namespace sc
{
    // 分析线程类
    class Counter
    {
    public:

        using FileReport = std::tuple<std::string, std::string, ReportItem>;

        Counter() = default;

    private:

        std::mutex m_Mutex;
        std::vector<std::pair<std::string, std::string>> m_Files;
        std::vector<FileReport> m_Reports;
        LangRules m_Rules;

        // 取出一个文件
        bool _PickFile(std::pair<std::string, std::string>& file);

        // 添加一个文件
        bool _AddFile(const std::filesystem::path& file, const std::vector<std::string>& langs, const std::string& excludes, bool allowEmpty);

        // 分析线程函数
        std::vector<FileReport> _Analyze(unsigned int mode);

    public:

        std::vector<std::string> Files() const;
        std::vector<std::string> Files(const std::string& language) const;
        const LangRules& Rules() const { return m_Rules; }

        // 加载文件
        unsigned int Load(const std::string& input, const std::string& config, const std::vector<std::string>& langs, const std::string& excludes, bool allowEmpty);

        // 启动线程
        bool Start(unsigned int nThread, unsigned int mode);

    };  // class Counter

}   // namespace sc
