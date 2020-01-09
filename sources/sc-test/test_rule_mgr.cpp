#include <set>
#include <fstream>

#include "../third/xf_simple_test.h"
#include "../third/json.hpp"

#include "../sc-counter/ReportType.h"
#include "../sc-counter/RuleManager.h"

namespace sc::test
{

    template<typename _Type>
    bool IsEqual(const std::vector<_Type>& a, const std::vector<_Type>& b)
    {
        if (a.size() != b.size())
            return false;

        std::set<_Type> x(a.begin(), a.end()), y(b.begin(), b.end());
        return (x.size() == y.size() &&
                std::equal(x.begin(), x.end(), y.begin()));
    }

    bool IsEqual(const sc::RuleManager& mgr, const std::string& name, const std::string& type, const sc::list_t& exts, const sc::syntax_t& syntax)
    {
        if (mgr.Contains(name))
        {
            const auto& [a, s] = mgr.GetRule(name);

            return (type == a
                    && IsEqual(std::get<0>(s), std::get<0>(syntax))
                    && IsEqual(std::get<1>(s), std::get<1>(syntax))
                    && IsEqual(std::get<2>(s), std::get<2>(syntax))
                    && IsEqual(std::get<3>(s), std::get<3>(syntax)));
        }

        return false;
    }

    _xfTest(test_rule)
    {
        sc::RuleManager mgr;
        std::string error("ok");
        auto b = mgr.Load("../../resources/config.json", error);
        std::cout << "result: " << (b ? "true" : "false") << ", " << error << std::endl;

        _xfExpect(IsEqual(mgr, "Java", "C", {}, { { "//" }, { {"/*", "*/"} }, { {"\"", "\""} }, {} }));
    }
}
