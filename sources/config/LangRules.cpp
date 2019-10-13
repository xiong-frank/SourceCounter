
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include "third/json.hpp"
#include "LangRules.h"

namespace sc
{

    bool LangRules::Load(const string_type& fromFile)
    {
        return false;
    }

    std::string LangRules::Type(const std::string& ext) const
    {
        return std::string();
    }

}