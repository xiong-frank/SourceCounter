#include <map>
#include <set>
#include <vector>
#include <fstream>

#include "../third/json.hpp"

#include "ReportType.h"
#include "RuleManager.h"

namespace sc
{

    RuleManager::RuleManager()
    {
        for (const auto& [name, rule] : _BuildInRule)
        {
            const auto& [exts, syntax] = rule;
            m_SyntaxMap.emplace(name, std::tuple<string_type, syntax_t>{ name, syntax });
            for (auto ext : exts)
                m_ExtMap.emplace(ext, name);
        }
    }

    bool RuleManager::Load(const string_type& filename)
    {
        std::ifstream fin(filename);
        if (fin.is_open())
        {
            nlohmann::json rules;
            fin >> rules;

            for (const auto& [key, value] : rules.items())
            {
                /*
                if (auto iter = _BuildInRule.find(key); iter != _BuildInRule.end())
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
                */
            }

            fin.close();

            return true;
        }

        return false;
    }

    list_t RuleManager::GetLanguages() const
    {
        list_t result;
        for (const auto& item : m_SyntaxMap)
            result.emplace_back(item.first);

        return result;
    }

    string_type RuleManager::GetLanguage(const string_type& ext) const
    {
        if (auto iter = m_ExtMap.find(ext); iter != m_ExtMap.end())
            return iter->second;

        return string_type();
    }

    void RuleManager::_AddRule(const string_type& name, const syntax_t& item, const list_type<string_type>& exts)
    {
        /*
        auto iter = m_SyntaxMap.find(name);
        if (iter != m_SyntaxMap.end())
            m_SyntaxMap.erase(iter);

        m_SyntaxMap.emplace(name, item);
        for (auto ext : exts)
            m_ExtMap[ext] = name;
            */
    }

    inline auto _make_symbol_for_ruby(const std::string& a, const std::string& b)
    {
        pairs_t symbols{ { R"(%q")", R"(")"} };
        for (auto i = 0; i < a.size(); ++i)
        {
            symbols.emplace_back(string_type("%q").append(1, a[i]), string_type(1, b[i]));
            symbols.emplace_back(string_type("%Q").append(1, a[i]), string_type(1, b[i]));
        }

        return symbols;
    }

    const std::map<std::string, std::tuple<list_t, syntax_t>> RuleManager::_BuildInRule{
        /*
        {"Java",        { { ".java" },
                          { { "//" },
                            { {"/*", "* /"} },
                            { { R"(")", R"(")" } },
                            { } } } },
        {"C#",          { { ".cs" },
                          { { "//" },
                            { {"/*", "* /"} },
                            { { R"(")", R"(")" } },
                            { { R"(@")", R"(")" } } } } },
        {"Python",      { { ".py" },
                          { { "#" },
                            { { R"(""")", R"(""")" }, { R"(''')", R"(''')" } },
                            { { R"(")", R"(")" }, { R"(')", R"(')" }, { R"(""")", R"(""")" }, { R"(''')", R"(''')" } },
                            { { R"(R")", R"(")" }, { R"(r")", R"(")" }, { R"(R')", R"(')" }, { R"(r')", R"(')" } } } } },
        {"JavaScript",  { { ".js" },
                          { { "//" },
                            { {"/*", "* /"} },
                            { { R"(")", R"(")" },{ "'", "'" }, { "`", "`" } },
                            { } } } },
        {"Clojure",     { { ".clj", ".cljc" },
                          { { ";", "#_" },
                            { { "(comment", ")" } },
                            { { R"(")", R"(")" } },
                            { } } } },
        {"ClojureScript", { { ".cljs" },
                          { { ";", "#_" },
                            { { "(comment", ")" } },
                            { { R"(")", R"(")" } },
                            { } } } },
        {"Rust",        {   },
        {"Shell",       {   },
        */
        {"C",           { { ".h", ".c" },
                          { { "//" },
                            { {"/*", "*/"} },
                            { { R"(")", R"(")" } },
                            { } } } },
        {"C++",         { { ".h", ".cpp", ".hpp", ".inl" },
                          { { "//" },
                            { {"/*", "*/"} },
                            { { R"(")", R"(")" } },
                            { { R"(R")", R"(")" }, { "(", ")" } } } } },
        {"Ruby",        { { ".rb" },
                          { { "#" },
                            { { "=begin", "=end" } },
                            { { R"(")", R"(")" }, { R"(')", R"(')" }, { R"(%q')", R"(')" }, { R"(%Q')", R"(')" }, { R"(%Q")", R"(")" } },
                            _make_symbol_for_ruby(R"({[<(`~!@#$%^&*:;-_+=,.\/ )", R"(}]>)`~!@#$%^&*:;-_+=,.\/ )") } } }
    };

    const string_type RuleManager::_DefaultName("C");

}
