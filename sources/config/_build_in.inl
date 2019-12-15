
const std::map<std::string, std::tuple<std::vector<std::string>, item_t>> _build_in_map{
    {"C++",         { { ".h", ".cpp", ".hpp", ".inl", ".cc" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { { R"x(R"()x", R"x()")x" } } } } },
    {"Java",        { { ".java" },
                      { { "//" },
                        { {"/*", "*/"} },
                        { { R"(")", R"(")" } },
                        { } } } },
    {"C",           { { ".h", ".c" },
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
                        { {"=begin", "=end"} },
                        { { R"(")", R"(")" }, { R"(')", R"(')" } },
                        { } } } },
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