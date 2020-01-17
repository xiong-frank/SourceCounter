/*
 * 测试规则管理器
 * 测试对规则配置加载的正确性以及对错误配置的识别。
 */

#include <set>
#include <fstream>

#include "../third/xf_simple_test.h"
#include "../third/json.hpp"

#include "../sc-counter/ReportType.h"
#include "../sc-counter/RuleManager.h"

namespace sc::test
{

    template<typename _Type>
    bool IsEqual(const _Type& a, const _Type& b)
    {
        return (a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin()));
    }

    inline bool IsEqual(const sc::RuleManager& mgr, const std::string& name, const std::string& type, const std::set<std::string>& exts, const sc::syntax_t& syntax)
    {
        if (mgr.Contains(name))
        {
            const auto& [a, s] = mgr.GetRule(name);
            auto extensions = mgr.GetExtensions(name);

            return (type == a
                    && IsEqual(exts, std::set<std::string>(extensions.begin(), extensions.end()))
                    && IsEqual(std::get<0>(s), std::get<0>(syntax))
                    && IsEqual(std::get<1>(s), std::get<1>(syntax))
                    && IsEqual(std::get<2>(s), std::get<2>(syntax))
                    && IsEqual(std::get<3>(s), std::get<3>(syntax)));
        }

        return false;
    }

    _xfTest(test_rule_build_in)
    {
        sc::RuleManager mgr;

        _xfExpect(mgr.Contains("C"));
        _xfExpect(mgr.Contains("C++"));
        _xfExpect(mgr.Contains("Ruby"));
        _xfExpect(mgr.Contains("Java"));
        _xfExpect(!mgr.Contains("Frank"));

        _xfExpect("C" == mgr.GetLanguage(".c"));
        _xfExpect("C++" == mgr.GetLanguage(".cpp"));
        _xfExpect("C++" == mgr.GetLanguage(".hpp"));
        _xfExpect("Ruby" == mgr.GetLanguage(".rb"));
        _xfExpect(mgr.GetLanguage(".bnb").empty());

        _xfExpect(IsEqual(mgr, "C", "C", { ".h", ".c" }, { { "//" }, { {"/*", "*/"} }, { {"\"", "\""} }, {} }));
        _xfExpect(IsEqual(mgr, "C++", "C++", { ".hpp", ".cpp", ".inl" }, { { "//" }, { {"/*", "*/"} }, { {"\"", "\""} }, { { R"(R")", R"(")" }, { "(", ")" } } }));
        _xfExpect(51 == std::get<3>(mgr.GetSyntax("Ruby")).size());
        _xfExpect(12 == std::get<2>(mgr.GetSyntax("Python")).size());
    }

    _xfTest(test_rule_load_file)
    {
        sc::RuleManager mgr;
        std::string strInfo("ok");

        _xfExpect(mgr.Load("../../resources/config.json", strInfo));
        _xfExpect("ok" == strInfo);

        _xfExpect(IsEqual(mgr, "C", "C++", { ".c" }, { { "#" }, { {"<!--", "-->"} }, { {"\"", "\""} }, {{"a", "b"}} }));
        _xfExpect(IsEqual(mgr, "C++", "C++", { ".inl", ".cpp" }, { { "//" }, { {"/*", "*/"} }, { {"\"", "\""} }, { { R"(R")", R"(")" }, { "(", ")" } } }));
        _xfExpect(IsEqual(mgr, "C++ Header", "C++", { ".h" }, { { "//" }, { {"/*", "*/"} }, { {"\"", "\""} }, { { R"(R")", R"(")" }, { "(", ")" } } }));
        _xfExpect(IsEqual(mgr, "Frank", "C", { ".cs" }, { { "//" }, { {"/*", "*/"} }, { {"\"", "\""} }, {} }));
        _xfExpect(IsEqual(mgr, "Java", "C", { ".java" }, { { "//" }, { {"/**", "**/"} }, { {"\"", "\""} }, {} }));
        _xfExpect(IsEqual(mgr, "Ruby", "Ruby", { ".rb" }, { { "#" }, { {"=begin", "=end"} }, { {"%q{", "}"} }, {{"%Q[", "]"}} }));
    }

    _xfTest(test_rule_error1)
    {
        sc::RuleManager mgr;
        std::string strInfo("ok");

        _xfExpect(!mgr.Load("../../resources/config-error1.json", strInfo));
        _xfExpect(std::string::npos != strInfo.find("exception"));
    }

    _xfTest(test_rule_error2)
    {
        sc::RuleManager mgr;
        std::string strInfo("ok");

        _xfExpect(!mgr.Load("../../resources/config-error2.json", strInfo));
        _xfExpect(R"(Analyzer "C--" of language "Ruby" is not support.)" == strInfo);
    }

    _xfTest(test_rule_error3)
    {
        sc::RuleManager mgr;
        std::string strInfo("ok");

        _xfExpect(!mgr.Load("../../resources/config-error3.json", strInfo));
        _xfExpect(R"(No file extension specified for language "Frank")" == strInfo);
    }

}
