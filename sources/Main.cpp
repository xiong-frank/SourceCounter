#include <vector>
#include <filesystem>

#include "third/xf_log_console.h"

#include "config/Option.h"

#include "counter/FileReport.h"
#include "counter/Rapporteur.h"

int main(int argc, char *argv[])
{
    if (sc::Option::ParseCommandLine(argv, argc))
    {
        auto t1 = std::chrono::system_clock::now();
        _sc_rapporteur.Start(_sc_opt.ThreadNumber());
        auto t2 = std::chrono::system_clock::now();

        _sc_rapporteur.Report(_sc_opt.OutputPath(), _sc_opt.Detail());

        std::cout << "spend time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
    }

    return 0;
}
