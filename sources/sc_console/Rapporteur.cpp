#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "../third/xf_log_console.h"
#include "../third/xf_cmd_parser.h"
#include "../third/json.hpp"

#include "Rapporteur.h"

namespace sc
{

    unsigned int _parser_detail(const std::string& detail)
    {
        if (detail.empty())
            return 0;

        std::map<std::string, unsigned int> order_map{
            { "files",      order_t::by_files },
            { "lines",      order_t::by_lines },
            { "codes",      order_t::by_codes },
            { "blanks",     order_t::by_blanks },
            { "comments",   order_t::by_comments },
            { "asc",        order_t::ascending },
            { "ascending",  order_t::ascending },
            { "des",        order_t::descending },
            { "descending", order_t::descending }
        };

        auto pos = detail.find(':', 3);
        if (std::string::npos == pos)
        {
            auto iter = order_map.find(detail);
            if (iter != order_map.end())
            {
                if (0 < (order_t::order_mask & iter->second))
                    return (iter->second | order_t::descending);
            }
        }
        else
        {
            auto a = detail.substr(0, pos);
            auto b = detail.substr(pos + 1);
            if (a != b)
            {
                auto iter1 = order_map.find(a);
                if (iter1 == order_map.end())
                    return 0;

                auto iter2 = order_map.find(b);
                if (iter2 == order_map.end())
                    return 0;

                if (0 < (order_t::order_mask & iter1->second) && 0 < (order_t::order_mask & iter2->second))
                    return 0;

                unsigned int order = (iter1->second | iter2->second);
                if (0 < (order_t::order_mask & order))
                    return order;
            }
        }

        return 0;
    }

    inline std::vector<std::string> _split_string(const std::string& str, char symbol)
    {
        std::vector<std::string> strs;

        size_t start = 0;
        for (size_t pos = str.find(symbol, start); std::string::npos != pos; pos = str.find(symbol, start))
        {
            if (start < pos)
                strs.emplace_back(str, start, pos - start);

            start = pos + 1;
        }

        if (start < str.size())
            strs.emplace_back(str, start);

        return strs;
    }

#include "_help_info.inl"

    bool _parse_option(Counter& counter, params_t& opt, const xf::cmd::result_t& result)
    {
        if (!result.is_valid())
        {
            std::cout << result.info() << std::endl;
            return false;
        }

        if (result.is_existing("--version"))
        {
            std::cout << app_name() << " " << version() << "  2019-10 by FrankXiong" << std::endl;
            return false;
        }

        if (result.is_existing("--help"))
        {
            if (result.has_value("--help"))
                _help::_show_help(result.get<std::string>("--help"));
            else
                _help::_show_help();

            return false;
        }

        opt.input = result.get<std::string>("--input");

        if (result.is_existing("--output"))
            opt.output = result.get<std::string>("--output");

        if (result.is_existing("--config"))
            opt.configFile = result.get<std::string>("--config");

        if (result.is_existing("--exclude"))
            opt.exclusion = result.get<std::string>("--exclude");

        if (result.is_existing("--mode"))
            opt.mode = result.get<unsigned int>("--mode");

        if (result.is_existing("--empty"))
            opt.allowEmpty = result.get<bool>("--empty");

        if (result.is_existing("--languages"))
            opt.languages = _split_string(result.get<std::string>("--languages"), ',');

        if (result.is_existing("--detail"))
        {
            if (result.has_value("--detail"))
                opt.detail = _parser_detail(result.get<std::string>("--detail"));
            else
                opt.detail = order_t::by_nothing;
        }

        if (0 == counter.Load(opt.input, opt.configFile, opt.exclusion, opt.languages, opt.allowEmpty))
        {
            std::cout << "counter load failed: No valid files are recognized by options." << std::endl;
            return false;
        }

        if (result.is_existing("--thread")) {
            opt.nThread = result.get<unsigned int>("--thread");
            if (0x20 < opt.nThread) opt.nThread = 0x20;
        }
        else {
            opt.nThread =   (counter.Files().size() < 0x0020 ? 0x01
                           : counter.Files().size() < 0x0040 ? 0x02
                           : counter.Files().size() < 0x0080 ? 0x04
                           : counter.Files().size() < 0x0200 ? 0x08
                           : counter.Files().size() < 0x0800 ? 0x10 : 0x20);
        }

        if (counter.Files().size() < opt.nThread)
            opt.nThread = (unsigned int)(counter.Files().size());

        if (result.is_existing("--explain"))
        {
            Explain(counter, opt);
            return false;
        }

        return true;
    }

    bool ParseCommandLine(Counter& counter, params_t& opt, const char* const* argv, unsigned int argc)
    {
        /*
         --help=--input
         --version
         --input=/home/dir
         --output=/home/file
         --config=/home/config.json
         --languages=c++,java,ruby
         --exclude=*.h
         --mode=123
         --detail=lines|asc
         --thread=10
         --empty=true
         --explain
        */
        if (argc < 2)
        {
            std::cout << R"(Missing command line arguments, please specify the necessary arguments or enter "SourceCounter --help" for help.)" << std::endl;
            return false;
        }

        xf::cmd::Parser parser;
        parser.AddOption({ _opt_keys(_sc_cmd_help), { xf::cmd::value_t::vt_string, true, false, false, [&parser](const std::string& k) { return parser.IsValid(k); } } })
            .AddOption({ _opt_keys(_sc_cmd_version), { true, false } })
            .AddOption({ _opt_keys(_sc_cmd_input), { xf::cmd::value_t::vt_string, false, true, true } })
            .AddOption({ _opt_keys(_sc_cmd_output), { xf::cmd::value_t::vt_string, false, false, true } })
            .AddOption({ _opt_keys(_sc_cmd_config), { xf::cmd::value_t::vt_string, false, false, true } })
            .AddOption({ _opt_keys(_sc_cmd_mode), { xf::cmd::value_t::vt_unsigned, false, false, true} })
            .AddOption({ _opt_keys(_sc_cmd_languages), { xf::cmd::value_t::vt_string, false, false, true } })
            .AddOption({ _opt_keys(_sc_cmd_exclude), { xf::cmd::value_t::vt_string, false, false, true } })
            .AddOption({ _opt_keys(_sc_cmd_detail), { xf::cmd::value_t::vt_string, false, false, false, [](const std::string& v) { return (0 < _parser_detail(v)); } } })
            .AddOption({ _opt_keys(_sc_cmd_empty), { xf::cmd::value_t::vt_boolean, false, false, true } })
            .AddOption({ _opt_keys(_sc_cmd_thread), { xf::cmd::value_t::vt_unsigned, false, false, true, "[1-9][0-9]?" } })
            .AddOption({ _opt_keys(_sc_cmd_explain), { false, false } });

        if (argc < 3)
            if (std::filesystem::exists(argv[1]))
                return _parse_option(counter, opt, parser.Parse({ "--input", argv[1] }));

        if (argc < 4)
        {
            if (std::filesystem::exists(argv[1]))
            {
                if (std::filesystem::exists(argv[2]))
                    return _parse_option(counter, opt, parser.Parse({ "--input", argv[1], "--output", argv[2] }));
                else
                    return _parse_option(counter, opt, parser.Parse({ "--input", argv[1], argv[2] }));
            }
        }

        return _parse_option(counter, opt, parser.Parse(argv, 1, argc));
    }

    inline std::string _make_detail_text(unsigned int detail)
    {
        std::string _order_str[]{ "", "by_nothing", "by_lines", "by_codes", "by_comments", "by_blanks", "by_files" };

        unsigned int order = (order_t::order_mask & detail);
        if (order_t::no_show == order || order_t::by_nothing == order)
            return _order_str[order];

        std::string _dir_str[]{ "ascending", "descending" };

        return _order_str[order] + " | " + _dir_str[(order_t::order_direction & detail) >> 3];
    }

    void Explain(const Counter& counter, const params_t& opt)
    {
        std::cout << std::setw(_help::_indent_number) << "input"        << ": " << opt.input << std::endl;
        std::cout << std::setw(_help::_indent_number) << "config file"  << ": " << opt.configFile << std::endl;
        std::cout << std::setw(_help::_indent_number) << "languages"    << ": " << xf::log::to_string(opt.languages) << std::endl;
        std::cout << std::setw(_help::_indent_number) << "exclusion"    << ": " << opt.exclusion << std::endl;
        std::cout << std::setw(_help::_indent_number) << "allow empty"  << ": " << (opt.allowEmpty ? "true" : "false") << std::endl;
        std::cout << std::setw(_help::_indent_number) << "files"        << ": " << counter.Files().size() << std::endl;
        std::cout << std::setw(_help::_indent_number) << "thread count" << ": " << opt.nThread << std::endl;
        std::cout << std::setw(_help::_indent_number) << "mode"         << ": " << opt.mode << std::endl;
        std::cout << std::setw(_help::_indent_number) << "output"       << ": " << opt.output << std::endl;
        std::cout << std::setw(_help::_indent_number) << "detail"       << ": " << _make_detail_text(opt.detail) << std::endl;
    }

    using view_report_t = std::tuple<report_t, unsigned int>;

    inline view_report_t& operator += (view_report_t& x, const report_t& y)
    {
        auto& [r, n] = x;
        auto& [x1, x2, x3, x4] = r;
        const auto& [y1, y2, y3, y4] = y;

        x1 += y1;
        x2 += y2;
        x3 += y3;
        x4 += y4;
        ++n;

        return x;
    }

    template<unsigned int _k>
    inline bool _LessThan(const view_report_t& a, const view_report_t& b) {
        return std::get<_k>(std::get<0>(a)) < std::get<_k>(std::get<0>(b));
    }

    inline bool _LessThanByFiles(const view_report_t& a, const view_report_t& b) { return std::get<1>(a) < std::get<1>(b); }
    inline bool _LessThanByLines(const view_report_t& a, const view_report_t& b) { return _LessThan<count_t::_lines>(a, b); }
    inline bool _LessThanByCodes(const view_report_t& a, const view_report_t& b) { return _LessThan<count_t::_codes>(a, b); }
    inline bool _LessThanByComments(const view_report_t& a, const view_report_t& b) { return _LessThan<count_t::_comments>(a, b); }
    inline bool _LessThanByBlanks(const view_report_t& a, const view_report_t& b) { return _LessThan<count_t::_blanks>(a, b); }

    using report_map_t = std::map<std::string, view_report_t>;
    using report_pair_t = report_map_t::value_type;

    inline nlohmann::json _to_json(const report_t& report)
    {
        return { {"Lines", std::get<count_t::_lines>(report) },
                 {"Codes", std::get<count_t::_lines>(report) },
                 {"Blanks", std::get<count_t::_lines>(report) },
                 {"Comments", std::get<count_t::_lines>(report) } };
    }

    inline nlohmann::json _to_json(const view_report_t& report)
    {
        const auto& [r, n] = report;
        auto j = _to_json(r);
        j.emplace("Files", n);
        return j;
    }

    inline bool _OutputToFile(const std::string& filename, const std::vector<Counter::file_report_t>& reports, const report_map_t& reportMap, const view_report_t& total)
    {
        std::ofstream fout(filename);
        if (fout.is_open())
        {
            nlohmann::json file_report;
            for (const auto& [name, type, report] : reports)
            {
                auto j = _to_json(report);
                j.emplace("Language", type);
                file_report.emplace(name, j);
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

    constexpr unsigned int _cell_width(10); // 单元格宽度

    inline void _ShowReport(const view_report_t& report)
    {
        const auto& [r, n] = report;
        std::cout << " | " << std::setw(_cell_width) << n
                  << " | " << std::setw(_cell_width) << std::get<count_t::_lines>(r)
                  << " | " << std::setw(_cell_width) << std::get<count_t::_codes>(r)
                  << " | " << std::setw(_cell_width) << std::get<count_t::_comments>(r)
                  << " | " << std::setw(_cell_width) << std::get<count_t::_blanks>(r)
                  << " |" << std::endl;
    }

    inline void _ShowReport(const std::string& name, const view_report_t& report)
    {
        std::cout << " | " << std::setw(_cell_width) << name;
        _ShowReport(report);
    }

    void OutputReport(const std::vector<Counter::file_report_t>& reports, const std::string& filename, unsigned int detail)
    {
        view_report_t total{ { 0, 0, 0, 0 }, 0 };
        report_map_t reportMap;
        for (const auto& [name, type, report] : reports)
        {
            total += report;

            auto iter = reportMap.find(type);
            if (iter == reportMap.end())
                reportMap.emplace(type, view_report_t{ report, 1 });
            else
                iter->second += report;
        }

        if (unsigned int rank = (order_t::order_mask & detail); 0 < rank)
        {
            std::list<report_pair_t> views;
            bool asc = (order_t::ascending == (order_t::order_direction & detail));

            for (const auto& item : reportMap)
                _InsertReport(views, item,
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

            std::cout << " +------------+------------+------------+------------+------------+------------+" << std::endl;
            std::cout << " |   Language |      Files |      Lines |      Codes |   Comments |     Blanks |" << std::endl;
            std::cout << " +------------+------------+------------+------------+------------+------------+" << std::endl;

            for (const auto& report : views)
                _ShowReport(report.first, report.second);

            std::cout << " +------------+------------+------------+------------+------------+------------+" << std::endl;
            _ShowReport("Total", total);
            std::cout << " +------------+------------+------------+------------+------------+------------+" << std::endl;
        }
        else
        {

            std::cout << " +------------+------------+------------+------------+------------+" << std::endl;
            std::cout << " |      Files |      Lines |      Codes |   Comments |     Blanks |" << std::endl;
            std::cout << " +------------+------------+------------+------------+------------+" << std::endl;
            _ShowReport(total);
            std::cout << " +------------+------------+------------+------------+------------+" << std::endl;
        }

        _OutputToFile(filename, reports, reportMap, total);
    }

}
