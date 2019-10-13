#include <vector>
#include <string>
#include <map>

#include "config/Option.h"
#include "config/LangRules.h"

namespace sc
{
    void Option::Explain(const std::vector<std::string>& file) const
    {
    }

    bool Option::ParseCommandLine(const char* const * argv, std::size_t argc)
    {

        _sc_lrs.Load(_sc_opt.ConfigFile());
        _sc_lrs.BuildInRules();

        return false;
    }

}