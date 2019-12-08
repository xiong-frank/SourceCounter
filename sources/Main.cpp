
#include <filesystem>

#include "third/xf_log_console.h"

#include "config/Option.h"

#include "counter/FileReport.h"
#include "counter/Rapporteur.h"

#include <map>
#include "config/LangRules.h"
#include "counter/Analyzer.h"

int main(int argc, char *argv[])
{
    if (sc::Option::ParseCommandLine(argv, argc))
    {
        sc::Analyzer a(*_sc_lrs.GetRule("C"));
        auto r = a.Analyze("demo.c");
        /*
        auto t1 = std::chrono::system_clock::now();
        _sc_rapporteur.Start(_sc_opt.ThreadNumber());
        auto t2 = std::chrono::system_clock::now();

        _sc_rapporteur.Report(_sc_opt.OutputPath(), _sc_opt.Detail());
        */

    }
    
    return 0;
}
