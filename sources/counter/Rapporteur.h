#pragma once

namespace sc
{
    class Analyzer;

    // 分析线程类
    class Rapporteur
    {
    private:

        std::mutex m_Mutex;
        std::vector<std::pair<std::string, std::string>> m_Files;
        std::vector<FileReport> m_Reports;

        Rapporteur() = default;
        Rapporteur(const Rapporteur&) = delete;
        Rapporteur& operator=(const Rapporteur&) = delete;

        // 取出一个文件
        bool _PickFile(std::pair<std::string, std::string>& file);

        // 添加一个文件
        bool _AddFile(const std::filesystem::path& file, const std::vector<std::string>& langs, const std::string& excludes);

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
        void Report(const std::string& filename, unsigned int detail) const;

        static Rapporteur& Instance() { static Rapporteur _rapporteur; return (_rapporteur); }

    };  // class Rapporteur

}   // namespace sc

#define _sc_rapporteur sc::Rapporteur::Instance()
