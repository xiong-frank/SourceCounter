

const std::map<std::string, std::tuple<std::vector<std::string>, item_t>> _build_in_map{
    {"C++", {{ ".h", ".cpp", ".hpp", ".inl", ".cc" },
             { {"//"}, { {"/*", "*/"} }, { { R"(")", R"(")" } }, { { R"x(R"()x", R"x()")x" } } } } }
};
