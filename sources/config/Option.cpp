#include <vector>
#include <string>
#include <map>
#include <filesystem>

#include "../third/xf_log_console.h"
#include "../third/xf_cmd_parser.h"

#include "Option.h"
#include "LangRules.h"

#include "../counter/FileReport.h"
#include "../counter/Rapporteur.h"

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

    bool Option::ParseCommandLine(const char* const* argv, unsigned int argc)
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
                return Instance()._parse_option(parser.Parse({ "--input", argv[1] }));

        if (argc < 4)
        {
            if (std::filesystem::exists(argv[1]))
            {
                if (std::filesystem::exists(argv[2]))
                    return Instance()._parse_option(parser.Parse({ "--input", argv[1], "--ouput", argv[2] }));
                else
                    return Instance()._parse_option(parser.Parse({ "--input", argv[1], argv[2] }));
            }
        }

        return Instance()._parse_option(parser.Parse(argv, 1, argc));
    }

    bool Option::_parse_option(const xf::cmd::result_t& result)
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

        input = result.get<std::string>("--input");

        if (result.is_existing("--output"))
            output = result.get<std::string>("--output");

        if (result.is_existing("--config"))
            configFile = result.get<std::string>("--config");

        if (result.is_existing("--exclude"))
            exclusion = result.get<std::string>("--exclude");

        if (result.is_existing("--mode"))
            mode = result.get<unsigned int>("--mode");

        if (result.is_existing("--empty"))
            empty = result.get<bool>("--empty");

        _sc_lrs.Load(ConfigFile());
        languages = _sc_lrs.GetLanguages();

        if (result.is_existing("--languages"))
        {
            languages = _split_string(result.get<std::string>("--languages"), ',');
            languages.erase(std::remove_if(languages.begin(), languages.end(), [](const auto& v) { return !_sc_lrs.IsSupport(v); }), languages.end());
        }

        if (languages.empty())
        {
            std::cout << "No valid language name matched." << std::endl;
            return false;
        }

        if (result.is_existing("--detail"))
        {
            if (result.has_value("--detail"))
                detail = _parser_detail(result.get<std::string>("--detail"));
            else
                detail = order_t::by_nothing;
        }

        _sc_rapporteur.Load(input, languages, exclusion);

        if (result.is_existing("--thread")) {
            nThread = result.get<unsigned int>("--thread");
            if (0x20 < nThread) nThread = 0x20;
        } else {
            nThread =   (_sc_rapporteur.Files().size() < 0x0020 ? 0x01
                       : _sc_rapporteur.Files().size() < 0x0040 ? 0x02
                       : _sc_rapporteur.Files().size() < 0x0080 ? 0x04
                       : _sc_rapporteur.Files().size() < 0x0200 ? 0x08
                       : _sc_rapporteur.Files().size() < 0x0800 ? 0x10 : 0x20);
        }

        if (_sc_rapporteur.Files().size() < nThread) nThread = unsigned int(_sc_rapporteur.Files().size());

        if (result.is_existing("--explain"))
        {
            Explain();
            return false;
        }

        return true;
    }

    inline std::string _make_detail_text(unsigned int detail) {
        std::string _order_str[]{"", "by_nothing", "by_lines", "by_codes", "by_comments", "by_blanks", "by_files" };

        unsigned int order = (order_t::order_mask & detail);
        if (order_t::no_show == order || order_t::by_nothing == order)
            return _order_str[order];

        std::string _dir_str[]{ "ascending", "descending" };

        return _order_str[order] + " | " + _dir_str[(order_t::order_direction & detail) >> 3];
    }

    void Option::Explain() const
    {
        std::cout << std::setw(_help::_indent_number) << "input"         << ": " << input << std::endl;
        std::cout << std::setw(_help::_indent_number) << "config file"   << ": " << configFile << std::endl;
        std::cout << std::setw(_help::_indent_number) << "languages"     << ": " << xf::log::to_string(languages) << std::endl;
        std::cout << std::setw(_help::_indent_number) << "exclusion"     << ": " << exclusion << std::endl;
        std::cout << std::setw(_help::_indent_number) << "allow empty"   << ": " << (empty ? "true" : "false") << std::endl;
        std::cout << std::setw(_help::_indent_number) << "files"         << ": " << _sc_rapporteur.Files().size() << std::endl;
        std::cout << std::setw(_help::_indent_number) << "thread number" << ": " << nThread << std::endl;
        std::cout << std::setw(_help::_indent_number) << "mode"          << ": " << mode << std::endl;
        std::cout << std::setw(_help::_indent_number) << "output"        << ": " << output << std::endl;
        std::cout << std::setw(_help::_indent_number) << "detail"        << ": " << _make_detail_text(detail) << std::endl;
    }

}
