#include <vector>
#include <map>

#include "third/xf_log_console.h"

#include "config/Option.h"
#include "counter/FileReport.h"
#include "counter/Rapporteur.h"

#include <filesystem>

int main(int argc, char *argv[])
{
    for (auto& iter : std::filesystem::recursive_directory_iterator("."))
    {
        if (iter.is_regular_file())
        {
            // std::regex_match(iter.path());

            std::cout << iter.path() << std::endl;
            std::cout << std::filesystem::canonical(iter.path()) << std::endl;
        }
    }

    /*
    if (sc::Option::ParseCommandLine(argv, argc))
    {
        _sc_rapporteur .Load(_sc_opt.InputPath(), _sc_opt.Languages(), _sc_opt.Exclusion());

        if (_sc_opt.Explaining())
        {
            _sc_opt.Explain(_sc_rapporteur.Files());

            return 0;
        }

        auto t1 = std::chrono::system_clock::now();
        _sc_rapporteur.Start(_sc_opt.ThreadNumber());
        auto t2 = std::chrono::system_clock::now();

        _sc_rapporteur.Report(_sc_opt.OutputPath(), _sc_opt.Detail());
    }
    */

    return 0;
}
