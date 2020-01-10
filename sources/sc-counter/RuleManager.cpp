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
        for (const auto& [name, rule] : _BuildInRules)
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
                auto analyzer = _BuildInRules.end();
                auto extension = _BuildInRules.end();

                if (auto type_iter = value.find("analyzer"); type_iter != value.end())
                {
                    auto type = type_iter->get<string_type>();
                    analyzer = _BuildInRules.find(type);
                    if (_BuildInRules.end() == analyzer)
                    {
                        error = R"(Analyzer ")" + type + R"(" of language ")" + key + R"(" is not support.)";
                        return false;
                    }

                    extension = _BuildInRules.find(key);
                }
                else
                {
                    analyzer = _BuildInRules.find(key);
                    if (_BuildInRules.end() == analyzer)
                        analyzer = _BuildInRules.find(_DefaultAnalyzer);
                    else
                        extension = analyzer;
                }

                if (auto exts_iter = value.find("extensions"); exts_iter == value.end())
                {
                    if (_BuildInRules.end() == extension)
                    {
                        error = R"(No file extension specified for language ")" + key + R"(")";
                        return false;
                    }
                }
                else
                {
                    if (_BuildInRules.end() != extension)
                        for (const auto& ext : std::get<0>(extension->second))
                            m_ExtMap.erase(ext);

                    for (const auto& ext : exts_iter->get<list_t>())
                        m_ExtMap[ext] = key;
                }
               
                if (auto iter = value.find("syntax"); iter != value.end())
                    m_SyntaxMap[key] = { analyzer->first, { (*iter)[0], (*iter)[1], (*iter)[2], (*iter)[3] } };
                else
                    m_SyntaxMap[key] = { analyzer->first, std::get<1>(analyzer->second) };
                    
            }
        } catch (const std::exception & ex) {
            error.assign("Read rule config occur exception:\n").append(ex.what());
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

    list_t RuleManager::GetExtensions(const string_type& name) const
    {
        list_t exts;
        for (const auto& [ext, value] : m_ExtMap)
            if (_StringEqual(value, name))
                exts.emplace_back(ext);

        return exts;
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


    inline auto _make_symbol_for_python(const list_t& prefixes, const list_t& quotes)
    {
        pairs_t symbols;
        for (const auto& prefix : prefixes)
            for (const auto& quote : quotes)
                symbols.emplace_back(prefix + quote, quote);

        for (const auto& quote : quotes)
            symbols.emplace_back(quote, quote);

        return symbols;
    }


    const std::map<std::string, std::tuple<list_t, syntax_t>> RuleManager::_BuildInRules{
        {"C",           { { ".h", ".c" },
                          { { "//" },
                            { {"/*", "*/"} },
                            { { R"(")", R"(")" } },
                            { } } } },
        {"C++",         { { ".cpp", ".hpp", ".inl" },
                          { { "//" },
                            { {"/*", "*/"} },
                            { { R"(")", R"(")" } },
                            { { R"(R")", R"(")" }, { "(", ")" } } } } },
        {"Java",       { { ".java" },
                          { { "//" },
                            { {"/*", "*/"} },
                            { { R"(")", R"(")" } },
                            { } } } },
        {"JavaScript",  { { ".js" },
                          { { "//" },
                            { {"/*", "*/"} },
                            { { R"(")", R"(")" },{ "'", "'" }, { "`", "`" } },
                            { } } } },
        {"C#",          { { ".cs" },
                          { { "//" },
                            { {"/*", "*/"} },
                            { { R"(")", R"(")" } },
                            { { R"(@")", R"(")" } } } } },
        {"Python",      { { ".py" },
                          { { "#" },
                            { { R"(""")", R"(""")" }, { R"(''')", R"(''')" } },
                            _make_symbol_for_python({"R", "r"},{R"(""")", R"(''')", R"(")", R"(')"}),
                            { } } } },
        {"Ruby",        { { ".rb" },
                          { { "#" },
                            { { "=begin", "=end" } },
                            { { R"(")", R"(")" }, { R"(')", R"(')" }, { R"(%q')", R"(')" }, { R"(%Q')", R"(')" }, { R"(%Q")", R"(")" } },
                            _make_symbol_for_ruby(R"({[<(`~!@#$%^&*:;-_+=,.\/ )", R"(}]>)`~!@#$%^&*:;-_+=,.\/ )") } } }
    };

    const string_type RuleManager::_DefaultAnalyzer("C");

}
