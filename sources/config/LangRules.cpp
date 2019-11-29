
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <iostream>

#include "third/json.hpp"
#include "LangRules.h"

namespace sc
{

    LangRules::LangRules()
    {
#include "_build_in.inl"

        for (const auto& [name, item] : _build_in_map)
        {
            m_ItemMap.emplace(name, std::get<1>(item));
            for (auto ext : std::get<0>(item))
                m_ExtMap.emplace(ext, name);
        }
    }

    LangRules::pairs_t _to_pairs(const nlohmann::json& v)
    {
        LangRules::pairs_t pairs;
        for (const auto& [k, v] : v.items())
            pairs.emplace_back(k, v.get<std::string>());

        return pairs;
    }

    bool LangRules::Load(const string_type& filename)
    {
        std::ifstream fin(filename);
        if (fin.is_open())
        {
            nlohmann::json rules;
            fin >> rules;

            std::set<string_type> _build_in_languages;
            for (const auto& item : m_ItemMap)
                _build_in_languages.emplace(item.first);

            for (const auto& [key, value] : rules.items())
            {
                if (_build_in_languages.find(key) == _build_in_languages.end())
                {
                    const auto& options = value.at("options");
                    m_ItemMap.emplace(key, item_t(options[0].get<list_t>(), _to_pairs(options[1]), _to_pairs(options[2]), _to_pairs(options[3])));
                }

                for (const auto& ext : value.at("extensions"))
                    m_ExtMap.emplace(ext.get<std::string>(), key);
            }

            fin.close();
        }

        return false;
    }

    const LangRules::item_t* LangRules::GetRule(const string_type& name) const
    {
        auto iter = m_ItemMap.find(name);
        if (iter == m_ItemMap.end())
            return nullptr;

        return &iter->second;
    }

    list_type<string_type> LangRules::GetLanguage() const
    {
        return list_type<string_type>();
    }

    string_type LangRules::GetLanguage(const string_type& ext) const
    {
        return string_type();
    }

    void LangRules::_AddItem(const string_type& name, const item_t& item, const list_type<string_type>& exts)
    {
        auto iter = m_ItemMap.find(name);
        if (iter != m_ItemMap.end())
            m_ItemMap.erase(iter);

        m_ItemMap.emplace(name, item);
        for (auto ext : exts)
            m_ExtMap[ext] = name;
    }

}