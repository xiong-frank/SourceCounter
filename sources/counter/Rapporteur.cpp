#include <thread>
#include <mutex>
#include <regex>
#include <map>
#include <iostream>
#include <filesystem>

#include "third/xf_log_console.h"

#include "config/Option.h"
#include "config/PathUtils.h"
#include "config/LangRules.h"

#include "counter/Analyzer.h"
#include "counter/FileReport.h"
#include "counter/Rapporteur.h"

namespace sc
{
    template<typename _Type>
    bool _contains(const std::vector<_Type>& vtr, const _Type& value)
    {
        for (const auto& v : vtr)
            if (v == value)
                return true;

        return false;
    }

    unsigned int Rapporteur::Load(const std::string& input, const std::vector<std::string>& langs, const std::string& excludes)
    {
        unsigned int n = 0;
        for (auto& iter : std::filesystem::recursive_directory_iterator(input))
        {
            if (iter.is_regular_file() && (_sc_opt.AllowEmpty() || 0 < iter.file_size()))
            {
                auto file(iter.path());
                if (!std::regex_match(file.generic_string(), std::regex(excludes)))
                {
                    auto t = _sc_lrs.Type(file.extension().generic_string());
                    if (!t.empty() && (langs.empty() || _contains(langs, t)))
                    {
                        m_Files.emplace_back(std::filesystem::canonical(file).generic_string(), t);
                        ++n;
                    }
                }
            }
        }

        return n;
    }

    bool Rapporteur::Start(unsigned int nThread)
    {
        if (0 < nThread && !m_Files.empty())
        {
            std::vector<std::thread> vtrThread;
            for (unsigned int i = 0; i < nThread; ++i)
                vtrThread.emplace_back(std::thread([this]() { this->_Analyze(); }));

            for (std::thread& t : vtrThread)
                t.join();

            return true;
        }

        return false;
    }

    class _report_t : public ReportItem {

        unsigned int nFiles{ 0 };

    public:

        _report_t() = default;
        explicit _report_t(const ReportItem& item) : ReportItem(item), nFiles(1) { }

        unsigned int Files() const { return nFiles; }

        _report_t& operator += (const ReportItem& item) {
            ReportItem::operator+=(item);
            ++nFiles;

            return *this;
        }

    };  // class _report_t

    inline bool _LessThanByFiles(const _report_t& a, const _report_t& b) { return a.Files() < b.Files(); }
    inline bool _LessThanByLines(const _report_t& a, const _report_t& b) { return a.Lines() < b.Lines(); }
    inline bool _LessThanByCodes(const _report_t& a, const _report_t& b) { return a.Codes() < b.Codes(); }
    inline bool _LessThanByComments(const _report_t& a, const _report_t& b) { return a.Comments() < b.Comments(); }
    inline bool _LessThanByBlanks(const _report_t& a, const _report_t& b) { return a.Blanks() < b.Blanks(); }

    using report_map_t = std::map<std::string, _report_t>;
    using report_pair_t = report_map_t::value_type;

    inline void _ShowReport(const std::string& name, const _report_t& report)
    {
        std::printf("| %-10s | %-10u | %-10u | %-10u | %-10u | %-10u |",
                    name.c_str(), report.Files(), report.Lines(), report.Codes(), report.Comments(), report.Blanks());
    }

    inline void _OutputToFile(const std::string& filename, const std::vector<FileReport>& reports, const report_map_t& reportMap, const _report_t& total)
    {

    }

    template<typename _LessType>
    void _InsertReport(std::list<report_pair_t> reports, const report_pair_t& item, _LessType lt)
    {

    }

    void Rapporteur::Report(const std::string& filename, unsigned int detail) const
    {
        _report_t total;
        report_map_t reportMap;

        for (const auto& item : m_Reports)
        {
            total += item.GetReport();

            auto iter = reportMap.find(item.GetType());
            if (iter == reportMap.end())
                reportMap.emplace(item.GetType(), _report_t(item.GetReport()));
            else
                iter->second += item.GetReport();
        }

        unsigned int rank = (order_t::rank_mask & detail);
        if (0 < rank)
        {
            std::list<report_pair_t> reports;
            bool asc = (order_t::ascending == (order_t::order_mask & detail));

            for (const auto& item : reportMap)
                _InsertReport(reports, item,
                              [&asc, rank](const report_pair_t& a, const report_pair_t& b)
                              {
                                  switch (rank)
                                  {
                                  case order_t::by_files:
                                      return (asc ? _LessThanByFiles(a.second, b.second) : _LessThanByFiles(b.second, a.second));
                                  case order_t::by_lines:
                                      return (asc ? _LessThanByLines(a.second, b.second) : _LessThanByLines(b.second, a.second));
                                  case order_t::by_codes:
                                      return (asc ? _LessThanByCodes(a.second, b.second) : _LessThanByCodes(b.second, a.second));
                                  case order_t::by_comments:
                                      return (asc ? _LessThanByComments(a.second, b.second) : _LessThanByComments(b.second, a.second));
                                  case order_t::by_blank:
                                      return (asc ? _LessThanByBlanks(a.second, b.second) : _LessThanByBlanks(b.second, a.second));
                                  default:
                                      return true;
                                  }
                              });

            std::cout << "+------------+------------+------------+------------+------------+------------+" << std::endl;
            std::cout << "|   Language |      Files |      Lines |      Codes |   Comments |     Blanks |" << std::endl;
            std::cout << "+------------+------------+------------+------------+------------+------------+" << std::endl;

            for (const auto& report : reports)
                _ShowReport(report.first, report.second);

            std::cout << "+------------+------------+------------+------------+------------+------------+" << std::endl;
            _ShowReport("Total", total);
            std::cout << "+------------+------------+------------+------------+------------+------------+" << std::endl;
        }
        else
        {

            std::cout << "+------------+------------+------------+------------+------------+" << std::endl;
            std::cout << "|      Files |      Lines |      Codes |   Comments |     Blanks |" << std::endl;
            std::cout << "+------------+------------+------------+------------+------------+" << std::endl;
            std::printf("| %-10u | %-10u | %-10u | %-10u | %-10u |",
                        total.Files(), total.Lines(), total.Codes(), total.Comments(), total.Blanks());
            std::cout << "+------------+------------+------------+------------+------------+" << std::endl;
        }

        _OutputToFile(filename, m_Reports, reportMap, total);
    }

    std::vector<std::string> Rapporteur::Files() const
    {
        std::vector<std::string> vtr;
        for (const auto& item : m_Files)
            vtr.push_back(item.first);

        return vtr;
    }

    std::vector<std::string> Rapporteur::Files(const std::string& language) const
    {
        std::vector<std::string> vtr;
        for (const auto& item : m_Files)
            if (item.second == language)
                vtr.push_back(item.first);

        return vtr;
    }

    bool Rapporteur::_PickFile(std::pair<std::string, std::string>& file)
    {
        /*
         * 取文件操作是互斥操作
         * 同一时刻只能有一个线程可以从文件队列取到文件
         * 如果队列为空则取文件失败，否则将取到的文件从参数带出，并将该文件从队列移除。
         */
        std::lock_guard<std::mutex> automtx(m_Mutex);

        if (m_Files.empty())
            return false;

        file = m_Files.back();
        m_Files.pop_back();

        return true;
    }

    Analyzer Rapporteur::_GetAnalyzer(const std::string& language)
    {
        return Analyzer();
    }

    void Rapporteur::_Analyze()
    {
        // 循环取文件
        for (std::pair<std::string, std::string> item; _PickFile(item); )
            m_Reports.emplace_back(_GetAnalyzer(item.second).Analyze(item.first));
    }

}
