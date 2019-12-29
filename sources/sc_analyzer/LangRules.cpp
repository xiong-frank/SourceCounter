
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>

#include "../third/json.hpp"
#include "LangRules.h"

namespace sc
{

#include "_build_in.inl"

    LangRules::LangRules()
    {
        for (const auto& [name, item] : _build_in_map)
        {
            m_ItemMap.emplace(name, std::get<1>(item));
            for (auto ext : std::get<0>(item))
                m_ExtMap.emplace(ext, name);
        }
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
                if (auto iter = _build_in_map.find(key); iter != _build_in_map.end())
                {
                    for (auto ext : std::get<0>(iter->second))
                        m_ExtMap.erase(ext);
                }
                else
                {
                    const auto& options = value.at("options");
                    m_ItemMap.emplace(key, item_t{ options[0], options[1], options[2], options[3] });
                }

                for (const auto& ext : value.at("extensions"))
                    m_ExtMap[ext.get<std::string>()] = key;
            }

            fin.close();
        }

        return false;
    }

    bool LangRules::IsSupport(const string_type& name) const
    {
        return (m_ItemMap.find(name) != m_ItemMap.end());
    }

    const LangRules::item_t* LangRules::GetRule(const string_type& name) const
    {
        auto iter = m_ItemMap.find(name);
        if (iter == m_ItemMap.end())
            return nullptr;

        return &iter->second;
    }

    LangRules::list_t LangRules::GetLanguages() const
    {
        list_t result;
        for (const auto& item : m_ItemMap)
            result.emplace_back(item.first);

        return result;
    }

    string_type LangRules::GetLanguage(const string_type& ext) const
    {
        if (auto iter = m_ExtMap.find(ext); iter != m_ExtMap.end())
            return iter->second;

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
