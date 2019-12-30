#include "test_utils.h"

namespace sc::test
{
    bool sc_test::_run(const std::string& key) const
    {
        return false;
    }

    unsigned int sc_test::Test() const
    {
        return 0;
    }

    unsigned int sc_test::Test(const std::vector<std::string>& names) const
    {
        return 0;
    }

}

int main()
{
    sc::test::sc_test::Instance().Test();
    return 0;
}
