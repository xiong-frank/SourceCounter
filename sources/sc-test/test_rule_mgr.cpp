#include <fstream>

#include "../third/xf_simple_test.h"
#include "../third/json.hpp"

#include "../sc-counter/ReportType.h"
#include "../sc-counter/RuleManager.h"

namespace sc::test
{

    _xfTest(test_json)
    {
        std::ifstream fin("../../resources/config.json");
        if (fin.is_open())
        {
            nlohmann::json j;
            fin >> j;

            _xfExpect(j.contains("C"));
            _xfExpect(!j.contains("D"));

        }
    }

}
