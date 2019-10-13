#pragma once

#include <mutex>

namespace sc
{
    class Analyzer;
    class FileReport;

    // 分析线程类
    class Rapporteur
    {
    private:

        enum order_t {
            by_nothing  = 0x01,
            by_lines    = 0x02,
            by_codes    = 0x03,
            by_comments = 0x04,
            by_blank    = 0x05,
            by_files    = 0x06,
            rank_mask   = 0x07,

            ascending   = 0x00,
            descending  = 0x08,
            order_mask  = 0x08
        };

        std::mutex m_Mutex;
        std::vector<std::pair<std::string, std::string>> m_Files;
        std::vector<FileReport> m_Reports;

        Rapporteur() = default;
        Rapporteur(const Rapporteur&) = delete;
        Rapporteur& operator=(const Rapporteur&) = delete;

        // 取一个文件
        bool _PickFile(std::pair<std::string, std::string>& file);

        // 分析线程函数
        void _Analyze();

        // 根据语言获取分析器
        static Analyzer _GetAnalyzer(const std::string& language);

    public:

        std::vector<std::string> Files() const;
        std::vector<std::string> Files(const std::string& language) const;

        // 加载文件
        unsigned int Load(const std::string& input, const std::vector<std::string>& langs, const std::string& excludes);

        // 启动线程
        bool Start(unsigned int nThread);

        // 汇报
        void Report(const std::string& output, unsigned int detail) const;

        static Rapporteur& Instance() { static Rapporteur _rapporteur; return (_rapporteur); }

    };  // class Rapporteur

}   // namespace sc

#define _sc_rapporteur sc::Rapporteur::Instance()
