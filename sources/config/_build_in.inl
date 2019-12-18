
inline auto _make_symbol_for_ruby(const std::string& a, const std::string& b)
{
    LangRules::pairs_t symbols{ { R"(%q")", R"(")"} };
    for (auto i = 0; i < a.size(); ++i)
    {
        symbols.emplace_back(string_type("%q").append(1, a[i]), string_type(1, b[i]));
        symbols.emplace_back(string_type("%Q").append(1, a[i]), string_type(1, b[i]));
    }

    return symbols;
}

const std::map<std::string, std::tuple<std::vector<std::string>, LangRules::item_t>> _build_in_map{
    {"C",           { { ".h", ".c" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { } } } },
    {"C++",         { { ".h", ".cpp", ".hpp", ".inl" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { { R"x(R"()x", R"x()")x" } } } } },
    {"Java",        { { ".java" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { } } } },
    {"C#",          { { ".cs" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { { R"(@")", R"(")" } } } } },
    {"Python",      { { ".py" },
                      { { "#" },
                        { { R"(""")", R"(""")" }, { R"(''')", R"(''')" } },
                        { { R"(")", R"(")" }, { R"(')", R"(')" }, { R"(""")", R"(""")" }, { R"(''')", R"(''')" } },
                        { { R"(R")", R"(")" }, { R"(r")", R"(")" }, { R"(R')", R"(')" }, { R"(r')", R"(')" } } } } },
    {"Ruby",        { { ".rb" },
                      { { "#" }, 
                        { { "=begin", "=end" } },
                        { { R"(")", R"(")" }, { R"(')", R"(')" }, { R"(%q')", R"(')" }, { R"(%Q')", R"(')" }, { R"(%Q")", R"(")" } },
                        _make_symbol_for_ruby(R"({[<(`~!@#$%^&*:;-_+=,.\/ )", R"(}]>)`~!@#$%^&*:;-_+=,.\/ )") } } },
    {"JavaScript",  { { ".js" },
                      { { "//" },
                        { {"/*", "*/"} },
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
    {"Rust",        { { ".h", ".cpp", ".hpp", ".inl", ".cc" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { { R"x(R"()x", R"x()")x" } } } } },
    {"Shell",       { { ".sh" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { { R"x(R"()x", R"x()")x" } } } } }
};
