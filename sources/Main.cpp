
#include <filesystem>

#include "third/xf_log_console.h"

#include "config/Option.h"

#include "counter/FileReport.h"
#include "counter/Rapporteur.h"

class A {
public:
    void func1() { std::cout << "A::func1" << std::endl; }
    virtual void func2() { std::cout << "A::func2" << std::endl; }
};

class B : public A {
public:
    void func1() { std::cout << "B::func1" << std::endl; }
    virtual void func2() { std::cout << "B::func2" << std::endl; }
};


int main(int argc, char *argv[])
{
    B b;
    A a = b;
    A* pa = &b;

    a.func1();
    a.func2();

    pa->func1();
    pa->func2();
    /*
    if (sc::Option::ParseCommandLine(argv, argc))
    {
        auto t1 = std::chrono::system_clock::now();
        _sc_rapporteur.Start(_sc_opt.ThreadNumber());
        auto t2 = std::chrono::system_clock::now();

        _sc_rapporteur.Report(_sc_opt.OutputPath(), _sc_opt.Detail());
    }
    */
    return 0;
}
