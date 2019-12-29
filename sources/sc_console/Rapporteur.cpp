#include "Rapporteur.h"

namespace sc
{

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
        std::cout << "| " << std::setw(_indent_number) << name.c_str()
            << " | " << std::setw(_indent_number) << report.Files()
            << " | " << std::setw(_indent_number) << report.Lines()
            << " | " << std::setw(_indent_number) << report.Codes()
            << " | " << std::setw(_indent_number) << report.Comments()
            << " | " << std::setw(_indent_number) << report.Blanks()
            << " |" << std::endl;
    }

    inline nlohmann::json _to_json(const ReportItem& item)
    {
        return { {"Lines", item.Lines() },
                 {"Codes", item.Codes() },
                 {"Blanks", item.Blanks() },
                 {"Comments", item.Comments() } };
    }

    inline nlohmann::json _to_json(const _report_t& item)
    {
        auto j = _to_json(ReportItem(item));
        j.emplace("Files", item.Files());
        return j;
    }

    inline bool _OutputToFile(const std::string& filename, const std::vector<FileReport>& reports, const report_map_t& reportMap, const _report_t& total)
    {
        std::ofstream fout(filename);
        if (fout.is_open())
        {
            nlohmann::json file_report;
            for (const auto& report : reports)
            {
                auto j = _to_json(report.GetReport());
                j.emplace("Language", report.GetType());
                file_report.emplace(report.GetFilePath(), j);
            }

            nlohmann::json lang_report;
            for (const auto& [lang, report] : reportMap)
                lang_report.emplace(lang, _to_json(report));

            nlohmann::json total_report = { { "total.report",    _to_json(total) },
                                            { "language.report", lang_report },
                                            { "file.report",     file_report } };

            fout << std::setw(4) << total_report << std::endl;

            fout.close();

            return true;
        }

        return false;
    }

    template<typename _LessType>
    void _InsertReport(std::list<report_pair_t>& reports, const report_pair_t& item, _LessType lt)
    {
        auto iter = reports.begin();
        for (; iter != reports.end(); ++iter)
            if (!lt(*iter, item))
                break;

        reports.insert(iter, item);
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

        if (unsigned int rank = (order_t::order_mask & detail); 0 < rank)
        {
            std::list<report_pair_t> reports;
            bool asc = (order_t::ascending == (order_t::order_direction & detail));

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
                                  case order_t::by_blanks:
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
            std::cout << "| " << std::setw(_indent_number) << total.Files()
                << " | " << std::setw(_indent_number) << total.Lines()
                << " | " << std::setw(_indent_number) << total.Codes()
                << " | " << std::setw(_indent_number) << total.Comments()
                << " | " << std::setw(_indent_number) << total.Blanks()
                << " |" << std::endl;
            std::cout << "+------------+------------+------------+------------+------------+" << std::endl;
        }

        _OutputToFile(filename, m_Reports, reportMap, total);
    }

}