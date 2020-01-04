#include <map>
#include <set>
#include <vector>
#include <fstream>

#include "../third/json.hpp"

#include "ReportType.h"
#include "Rules.h"

namespace sc
{

#include "_build_in.inl"

    Rules::Rules()
    {
        for (const auto& [name, item] : _build_in_map)
        {
            m_SyntaxMap.emplace(name, std::get<1>(item));
            for (auto ext : std::get<0>(item))
                m_ExtMap.emplace(ext, name);
        }
    }

    bool Rules::Load(const string_type& filename)
    {
        std::ifstream fin(filename);
        if (fin.is_open())
        {
            nlohmann::json rules;
            fin >> rules;

            std::set<string_type> _build_in_languages;
            for (const auto& item : m_SyntaxMap)
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
                    m_SyntaxMap.emplace(key, syntax_t{ options[0], options[1], options[2], options[3] });
                }

                for (const auto& ext : value.at("extensions"))
                    m_ExtMap[ext.get<std::string>()] = key;
            }

            fin.close();
        }

        return false;
    }

    bool Rules::IsSupport(const string_type& name) const
    {
        return (m_SyntaxMap.find(name) != m_SyntaxMap.end());
    }

    const syntax_t* Rules::GetSyntax(const string_type& name) const
    {
        auto iter = m_SyntaxMap.find(name);
        if (iter == m_SyntaxMap.end())
            return nullptr;

        return &iter->second;
    }

    list_t Rules::GetLanguages() const
    {
        list_t result;
        for (const auto& item : m_SyntaxMap)
            result.emplace_back(item.first);

        return result;
    }

    string_type Rules::GetLanguage(const string_type& ext) const
    {
        if (auto iter = m_ExtMap.find(ext); iter != m_ExtMap.end())
            return iter->second;

        return string_type();
    }

    void Rules::_AddRule(const string_type& name, const syntax_t& item, const list_type<string_type>& exts)
    {
        auto iter = m_SyntaxMap.find(name);
        if (iter != m_SyntaxMap.end())
            m_SyntaxMap.erase(iter);

        m_SyntaxMap.emplace(name, item);
        for (auto ext : exts)
            m_ExtMap[ext] = name;
    }

}
