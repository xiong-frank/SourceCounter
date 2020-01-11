#include <chrono>
#include <iostream>

#include "Rapporteur.h"

int main(int argc, char *argv[])
{
    sc::params_t opt;
    sc::Counter _counter;

    if (sc::ParseCommandLine(_counter, opt, argv, argc))
    {
        auto t1 = std::chrono::system_clock::now();
        _counter.Start(opt.nThread, opt.mode);
        auto t2 = std::chrono::system_clock::now();

        sc::OutputReport(_counter.Reports(), opt.output, opt.view);

        std::cout << "spend time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
    }

    return 0;
}
