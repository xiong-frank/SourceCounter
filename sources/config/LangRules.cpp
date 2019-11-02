
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

    const LangRules::Item* LangRules::GetRule(const string_type& name) const
    {
        auto iter = m_ItemMap.find(name);
        if (iter == m_ItemMap.end())
            return nullptr;

        return &iter->second;
    }

    string_type LangRules::Type(const string_type& ext) const
    {
        return string_type();
    }

}