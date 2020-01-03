#include "../third/xf_simple_test.h"

#include "../sc_analyzer/ReportType.h"
#include "../sc_analyzer/analyzer/Analyzer.h"

namespace sc::test
{

    const char* _file_c = "../resources/demo.c";
    const char* _file_cpp = "../resources/demo.cpp";
    const char* _file_java = "../resources/demo.java";
    const char* _file_ruby = "../resources/demo.rb";
    const char* _file_python = "../resources/demo.py";
    const char* _file_csharp = "../resources/demo.cs";
    const char* _file_clojure = "../resources/demo.clj";

    _xfTest(test_c)
    {
        const std::string type("C");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::cc_is_comment | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        _xfExpect(sc::report_t{} == sc::Analyzer::Analyze(_file_c, type, {}, m1));
        _xfExpect(sc::report_t{} == sc::Analyzer::Analyze(_file_c, type, {}, sc::mode_t::default_mode));
        _xfExpect(sc::report_t{} == sc::Analyzer::Analyze(_file_c, type, {}, sc::mode_t::full_mode));
    }

    _xfTest(test_cpp)
    {
        const std::string type("C++");
        auto m1 = sc::mode_t::cc_is_code | sc::mode_t::cc_is_comment | sc::mode_t::mc_is_blank | sc::mode_t::ms_is_code;
        _xfExpect(sc::report_t{} == sc::Analyzer::Analyze(_file_cpp, type, {}, m1));
        _xfExpect(sc::report_t{} == sc::Analyzer::Analyze(_file_cpp, type, {}, sc::mode_t::default_mode));
        _xfExpect(sc::report_t{} == sc::Analyzer::Analyze(_file_cpp, type, {}, sc::mode_t::full_mode));
    }

}
