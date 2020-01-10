#include "../third/xf_simple_test.h"

#include "../sc-counter/ReportType.h"
#include "../sc-counter/RuleManager.h"
#include "../sc-counter/analyzer/Analyzer.h"

namespace sc::test
{

    inline auto _MakeReport(unsigned int a, unsigned int b, unsigned int c, unsigned int d) { return sc::report_t{a, b, c, d}; }
    inline const auto& GetRule(const std::string& name) { static sc::RuleManager _mgr; return _mgr.GetRule(name); }

    _xfTest(test_c)
    {
        const std::string file("../../resources/demo.c");
        const auto& [type, syntax] = GetRule("C");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;

        _xfExpect(_MakeReport(14, 7, 4, 3) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(_MakeReport(14, 7, 6, 3) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(_MakeReport(14, 7, 7, 3) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

    _xfTest(test_cpp)
    {
        const std::string file("../../resources/demo.cpp");
        const auto& [type, syntax] = GetRule("C++");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        auto m2 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_blank;

        _xfExpect(_MakeReport(18, 15, 1, 2) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(_MakeReport(18, 14, 1, 3) == sc::Analyzer::Analyze(file, type, syntax, m2));
        _xfExpect(_MakeReport(18, 15, 2, 2) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(_MakeReport(18, 15, 2, 3) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

    _xfTest(test_ruby)
    {
        const std::string file("../../resources/demo.rb");
        const auto& [type, syntax] = GetRule("Ruby");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        auto m2 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_blank;

        _xfExpect(_MakeReport(40, 30, 4, 6) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(_MakeReport(40, 28, 4, 8) == sc::Analyzer::Analyze(file, type, syntax, m2));
        _xfExpect(_MakeReport(40, 30, 5, 6) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(_MakeReport(40, 30, 7, 8) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

    _xfTest(test_python)
    {
        const std::string file("../../resources/demo.py");
        const auto& [type, syntax] = GetRule("Python");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        auto m2 = sc::mode_t::cc_is_code | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_blank;

        _xfExpect(_MakeReport(23, 13, 5, 5) == sc::Analyzer::Analyze(file, type, syntax, m1));
        _xfExpect(_MakeReport(23, 12, 5, 6) == sc::Analyzer::Analyze(file, type, syntax, m2));
        _xfExpect(_MakeReport(23, 13, 6, 5) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::default_mode));
        _xfExpect(_MakeReport(23, 13, 7, 6) == sc::Analyzer::Analyze(file, type, syntax, sc::mode_t::full_mode));
    }

}
