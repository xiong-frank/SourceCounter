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

    bool RuleManager::Load(const string_type& filename, string_type& error)
    {
        std::ifstream fin(filename);
        if (!fin.is_open())
        {
            error = R"(open config file: ")" + filename + R"(" failed.)";
            return false;
        }

        try {
            nlohmann::json rules;
            fin >> rules;

            for (const auto& [key, value] : rules.items())
            {
                auto analyzer = _BuildInRule.end();
                auto extension = _BuildInRule.end();
                auto type_iter = value.find("analyzer");
                if (type_iter != value.end())
                {
                    auto type = type_iter->get<string_type>();
                    analyzer = _BuildInRule.find(type);
                    if (_BuildInRule.end() == analyzer)
                    {
                        error = R"(Analyzer ")" + type + R"(" of language ")" + key + R"(" is not support.)";
                        return false;
                    }

                    extension = _BuildInRule.find(key);
                }
                else
                {
                    analyzer = _BuildInRule.find(key);
                    if (_BuildInRule.end() == analyzer)
                        analyzer = _BuildInRule.find(_DefaultName);
                    else
                        extension = analyzer;
                }

                auto exts_iter = value.find("extensions");
                if (exts_iter == value.end())
                {
                    if (_BuildInRule.end() == extension)
                    {
                        error = R"(No file extension specified for language ")" + key + R"(")";
                        return false;
                    }

                    for (auto ext : std::get<0>(extension->second))
                        m_ExtMap[ext] = key;
                }
                else
                {
                    if (_BuildInRule.end() != extension)
                    {
                        for (auto ext : std::get<0>(extension->second))
                            m_ExtMap.erase(ext);
                    }

                    for (const auto& ext : exts_iter->get<list_t>())
                        m_ExtMap[ext] = key;
                }
               
                if (auto iter = value.find("syntax"); iter != value.end())
                    m_SyntaxMap[key] = { analyzer->first, { (*iter)[0], (*iter)[1], (*iter)[2], (*iter)[3] } };
                else
                    m_SyntaxMap[key] = { analyzer->first, std::get<1>(analyzer->second) };
                    
            }
        } catch (const std::exception & ex) {
            error = "Read rule config occur exception:\n";
            error.append(ex.what());
            return false;
        }

        fin.close();
        return true;
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
        for (std::size_t i = 0; i < a.size(); ++i)
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
