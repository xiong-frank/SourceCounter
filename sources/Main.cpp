
#include <filesystem>

#include "third/xf_log_console.h"
#include "third/xf_cmd_parser.h"

#include "config/Option.h"
#include "config/LangRules.h"
#include "counter/FileReport.h"
#include "counter/Rapporteur.h"

int main(int argc, char *argv[])
{



    /*
    bool _ParseCommandLine(const char* const*, unsigned int);

    if (_ParseCommandLine(argv, argc))
    {
        auto t1 = std::chrono::system_clock::now();
        _sc_rapporteur.Start(_sc_opt.ThreadNumber());
        auto t2 = std::chrono::system_clock::now();

        _sc_rapporteur.Report(_sc_opt.OutputPath(), _sc_opt.Detail());
    }
    */
    return 0;
}

namespace sc {
    unsigned int _parser_detail(const std::string&);
}

bool _ParseCommandLine(const char* const* argv, unsigned int argc)
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
    parser.AddOption({ {"--help", "-h"}, { xf::cmd::value_t::vt_string, true, false, false, [&parser](const std::string& k) { return parser.IsValid(k); } } })
        .AddOption({ {"--version", "-v"}, { true, false } })
        .AddOption({ {"--input", "-i"}, { xf::cmd::value_t::vt_string, false, true, true } })
        .AddOption({ {"--output", "-o"}, { xf::cmd::value_t::vt_string, false, false, true } })
        .AddOption({ {"--config", "-c"}, { xf::cmd::value_t::vt_string, false, false, true } })
        .AddOption({ {"--mode", "-m"}, { xf::cmd::value_t::vt_unsigned, false, false, true} })
        .AddOption({ {"--languages", "-l"}, { xf::cmd::value_t::vt_string, false, false, true } })
        .AddOption({ {"--exclude", "-e"}, { xf::cmd::value_t::vt_string, false, false, true } })
        .AddOption({ {"--detail", "-d"}, { xf::cmd::value_t::vt_string, false, false, false, [](const std::string& v) { return (0 < sc::_parser_detail(v)); } } })
        .AddOption({ {"--empty"}, { xf::cmd::value_t::vt_boolean, false, false, true } })
        .AddOption({ {"--thread", "-t"}, { xf::cmd::value_t::vt_unsigned, false, false, true, "[1-9]{1,2}" } })
        .AddOption({ {"--explain", "-x"}, { false, false } });

    if (argc < 3)
        if (std::filesystem::exists(argv[1]))
            return _sc_opt.InitOption(parser.Parse({ "--input", argv[1] }));

    if (argc < 4)
    {
        if (std::filesystem::exists(argv[1]))
        {
            if (std::filesystem::exists(argv[2]))
                return _sc_opt.InitOption(parser.Parse({ "--input", argv[1], "--ouput", argv[2] }));
            else
                return _sc_opt.InitOption(parser.Parse({ "--input", argv[1], argv[2] }));
        }
    }

    return _sc_opt.InitOption(parser.Parse(argv, 1, argc));
}

