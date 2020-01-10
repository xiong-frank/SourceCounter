#include "../third/xf_simple_test.h"

#include "../sc-counter/Counter.h"

namespace sc::test
{

    inline auto _MakeReport(unsigned int a, unsigned int b, unsigned int c, unsigned int d) { return sc::report_t{ a, b, c, d }; }
    inline const auto& GetRule(const std::string& name) { static sc::RuleManager _mgr; return _mgr.GetRule(name); }

    _xfTest(test_counter)
    {
        sc::Counter counter;

        auto languages = counter.RuleMgr().GetLanguages();
        _xfExpect(6 == counter.LoadFile("../../resources", "", languages, true));

        counter.Start(2, sc::mode_t::default_mode);

        _xfExpect(6 == counter.Reports().size());

    }

}
