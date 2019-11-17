
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include "third/json.hpp"
#include "LangRules.h"

namespace sc
{
    inline std::pair<string_type, LangRules::item_t> _make_item_for_cpp() {
        return { string_type("C++"), { {"//"}, { {"/*", "*/"} }, { { R"(")", R"(")" } }, { { R"x(R"()x", R"x()")x" } } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_c() {
        return { string_type("C"), { {"//"}, { {"/*", "*/"} }, { { R"(")", R"(")" } }, { } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_java() {
        return { string_type("Java"), { {"//"}, { {"/*", "*/"} }, { { R"(")", R"(")" } }, { } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_csharp() {
        return { string_type("C#"), { {"//"}, { {"/*", "*/"} }, { { R"(")", R"(")" } }, { { R"(@")", R"(")" } } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_python() {
        return { string_type("Python"),
            { {"#"},
                               { { R"(""")", R"(""")" }, { R"(''')", R"(''')" } },
                               { { R"(")", R"(")" }, { R"(')", R"(')" }, { R"(""")", R"(""")" }, { R"(''')", R"(''')" } },
                               { { R"(R")", R"(")" }, { R"(r")", R"(")" }, { R"(R')", R"(')" }, { R"(r')", R"(')" } } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_ruby() {
        return { string_type("Ruby"), { {"#"}, { {"=begin", "=end"} }, { { R"(")", R"(")" }, { R"(')", R"(')" } }, { } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_javascript() {
        return { string_type("JavaScript"), { {"//"}, { {"/*", "*/"} }, { { R"(")", R"(")" }, { "'", "'" }, { "`", "`" } }, { } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_clojure() {
        return { string_type("Clojure"), { { ";", "#_" }, { { "(comment", ")" } }, { { R"(")", R"(")" } }, { } } };
    }

    inline std::pair<string_type, LangRules::item_t> _make_item_for_clojurescript() {
        return { string_type("ClojureScript"), { { ";", "#_" }, { { "(comment", ")" } }, { { R"(")", R"(")" } }, { } } };
    }

    LangRules::LangRules()
    {
#include "_build_in.inl"

        for (const auto& [name, item ] : _build_in_map)
        {

        }


        m_ItemMap.emplace(_make_item_for_cpp());
        m_ItemMap.emplace(_make_item_for_c());
        m_ItemMap.emplace(_make_item_for_csharp());
        m_ItemMap.emplace(_make_item_for_java());
        m_ItemMap.emplace(_make_item_for_javascript());
        m_ItemMap.emplace(_make_item_for_ruby());
        m_ItemMap.emplace(_make_item_for_python());
        m_ItemMap.emplace(_make_item_for_clojure());
        m_ItemMap.emplace(_make_item_for_clojurescript());

        for (auto ext : { ".h", ".cpp", ".hpp", ".inl", ".cc" })
            m_ExtMap.emplace(ext, "C++");

        for (auto ext : { ".h", ".c" })
            m_ExtMap.emplace(ext, "C");

        for (auto ext : { ".cs" })
            m_ExtMap.emplace(ext, "C#");

        for (auto ext : { ".java" })
            m_ExtMap.emplace(ext, "Java");

        for (auto ext : { ".py" })
            m_ExtMap.emplace(ext, "Python");

        for (auto ext : { ".rb" })
            m_ExtMap.emplace(ext, "Ruby");

        for (auto ext : { ".js" })
            m_ExtMap.emplace(ext, "Javascript");

        for (auto ext : { ".clj", ".cljc" })
            m_ExtMap.emplace(ext, "Clojure");

        for (auto ext : { ".cljs" })
            m_ExtMap.emplace(ext, "ClojureScript");
    }

    bool LangRules::Load(const string_type& fromFile)
    {
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