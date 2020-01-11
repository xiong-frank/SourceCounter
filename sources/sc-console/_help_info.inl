
inline const char* version() { return "1.0.0-snapshot"; }
inline const char* app_name() { return "SourceCounter"; }

namespace _help
{
    enum _sc_cmd {
        _sc_cmd_help,
        _sc_cmd_version,
        _sc_cmd_input,
        _sc_cmd_output,
        _sc_cmd_config,
        _sc_cmd_mode,
        _sc_cmd_languages,
        _sc_cmd_exclude,
        _sc_cmd_thread,
        _sc_cmd_explain,
        _sc_cmd_analyzer,
        _sc_cmd_view,
        _sc_cmd_empty
    };

    struct _help_item {
        std::vector<std::string> _names;
        std::string _summary;
        std::string _description;
        std::string _usage;
        std::string _example;
    };

    constexpr std::size_t _indent_number(0x0f);

    inline std::map<std::string, unsigned int> _make_cmd_map(const std::map<unsigned int, _help_item>& item_map) {
        std::map<std::string, unsigned int> _map;

        for (const auto& [_id, _item] : item_map)
            for (const auto& name : _item._names)
                _map.emplace(name, _id);

        return _map;
    }

    const std::map<unsigned int, _help_item> _help_map{
        { _sc_cmd::_sc_cmd_help, _help_item({{"--help", "-h"}, 
        "For help information.",
        "",
        "sc --help [command]",
        R"(sc --help
                 sc --help --input
                 sc --help -c)" })},

        { _sc_cmd::_sc_cmd_version, _help_item({{"--version", "-v"},
        "Show version information.",
        "",
        "sc --version",
        "sc --version" })},

        { _sc_cmd::_sc_cmd_input, _help_item({{"--input", "-i"},
        "[required] Specify the path of the source to be counted, can be a file or directory.",
        "",
        "sc --input [path]",
        R"(sc --input ./main.cpp
                 sc --input ./sources/)" })},

        { _sc_cmd::_sc_cmd_output, _help_item({{"--output", "-o"},
        "[optional] Specify the file path for the output statistics, the output is in JSON format.",
        R"(Contains the statistics of each source file and the summarized statistics.
                 If not specified, it will not be output.)",
        "sc --output [path]",
        "sc --output ./output/sc.json" })},

        { _sc_cmd::_sc_cmd_config, _help_item({{"--config", "-c"},
        "[optional] Specifies the config path for language syntax rules.",
        R"(If not specified, only built-in rules are used.
                 The config is in JSON format.
                 Example:
                    {
                        "Java": {
                                "extensions": [".java"],
                                "analyzer": "C",
                                "syntax": [ ["//"],
                                            [["/*", "*/"]],
                                            [["\"", "\""]],
                                            [] 
                                          ]
                            }
                    })",
        "sc --config [path]",
        "sc --config ./languages.json" })},

        { _sc_cmd::_sc_cmd_mode, _help_item({{"--mode", "-m"},
        "[optional] Specify statistical rules to explain the ambiguous lines.",
        R"(Ambiguous lines refers to a line contains multiple situations.
                 Specifically refer to:
                   Case1: There are code and comments in one line.
                   Case2: Empty lines in multiple lines of comments.
                   Case3: Empty lines in multiple lines of strings.

                 Specify "mode" for ambiguous lines to get clear statistical results.
                 The value of "mode" are as follows:

                   name            value   explain
                   --------------------------------------------------
                   cc_is_code      1       Case1 is Code Line.
                   cc_is_comment   2       Case1 is Comment Line.
                   mc_is_blank     4       Case2 is Blank Line.
                   mc_is_comment   8       Case2 is Comment Line.
                   ms_is_blank     16      Case3 is Blank Line.
                   ms_is_code      32      Case3 is Code Line.

                 Different rules can be used in combination.
                 Use the result of bit-or (or addition) as the value of "mode".
                 If not specified, the default value is 37.)",
        "sc --mode [number]",
        "sc --mode 53" })},

        { _sc_cmd::_sc_cmd_languages, _help_item({{"--languages", "-l"},
        "[optional] Specify the language for Statistics.",
        "Multiple languages are separated by commas,and language names are not case sensitive.",
        "sc --languages [name list]",
        "sc --languages C++,Java,Python" })},

        { _sc_cmd::_sc_cmd_exclude, _help_item({{"--exclude", "-e"},
        "[optional] A regular expression, specify paths to exclude.",
        "File paths that match the regular expression are not counted.",
        "sc --exclude [regular expression]",
        "sc --exclude *.h" })},

        { _sc_cmd::_sc_cmd_view, _help_item({{"--view"},
        "[optional] Show statistics by language, and you can also specify sorting rules.",
        R"(Sort columns: files, lines, codes, comments, blanks.
                 Sort order: asc[ending], des[cending])",
        "sc --view [command]",
        R"(sc --view
                 sc --view codes:asc)" })},

        { _sc_cmd::_sc_cmd_analyzer, _help_item({{"--analyzer", "-a"},
        "[optional] Show built-in analyzer information.",
        R"(You can specify a name to view the corresponding analyzer information.
                  If not specified, show all analyzer information.)",
        "sc --analyzer [command]",
        R"(sc --analyzer
                 sc --analyzer C++)" })},

        { _sc_cmd::_sc_cmd_empty, _help_item({{"--empty"},
        "[optional] Specifies whether to count empty files. default: true.",
        "",
        "sc --empty [bool]",
        "sc --empty true" })},

        { _sc_cmd::_sc_cmd_thread, _help_item({{"--thread", "-t"},
        "[optional] suggested number of threads used.",
        R"(The program automatically adjusts the number of threads based on the number of files it recognizes.
                 If the number of threads specified is too large, will be ignored.)",
        "sc --thread [number]",
        "sc --thread 4" })},

        { _sc_cmd::_sc_cmd_explain, _help_item({{"--explain", "-x"},
        "[optional] explain parameters to be used during program execution.",
        "If this option is specified, statistical process will not be excuted.",
        "sc --explain",
        "sc --explain" })}
    };

    const std::map<std::string, unsigned int> _cmd_map = _make_cmd_map(_help_map);

    std::set<std::string> _get_options(unsigned int k)
    {
        const auto& keys = _help::_help_map.at(k)._names;
        return { keys.begin(), keys.end() };
    }

    inline std::string _to_string(const std::vector<std::string>& strs)
    {
        if (strs.empty()) return std::string();

        std::string str = strs[0];
        for (std::size_t i = 1; i < strs.size(); str.append(", ").append(strs[i++]));

        return str;
    }

    inline void _show_help(const std::string& k) {
        const auto& [_names, _summary, _description, _usage, _example] = _help_map.at(_cmd_map.at(k));
        std::cout << std::endl;
        std::cout << std::right << std::setw(_indent_number) << _to_string(_names) << ": " << _summary << std::endl;
        if (!_description.empty()) std::cout << std::string(_indent_number + 2, ' ') << _description << std::endl;
        std::cout << std::endl;
        std::cout << std::setw(_indent_number) << "usage" << ": " << _usage << std::endl;
        std::cout << std::endl;
        std::cout << std::setw(_indent_number) << "example" << ": " << _example << std::endl;
        std::cout << std::endl;
    }

    inline void _show_help() {
        std::cout << std::endl;
        std::cout << R"(Enter the "--help [command]" for more information.)" << std::endl;
        std::cout << std::endl;

        for (const auto& h : _help_map)
            std::cout << std::right << std::setw(_indent_number) << _to_string(h.second._names) << "  " << h.second._summary << std::endl;
        
        std::cout << std::endl;
    }

    inline void _show_analyzer(const std::pair<std::string, std::tuple<sc::list_t, sc::syntax_t>>& analyzer)
    {
        const auto& [name, rule] = analyzer;
        const auto& [exts, syntax] = rule;
        std::cout << _make_filler<4, ' '>() << R"(")" << name << R"(": {)" << std::endl;
        std::cout << _make_filler<8, ' '>() << R"("extensions": )" << nlohmann::json(exts).dump() << "," << std::endl;
        std::cout << _make_filler<8, ' '>() << R"("syntax": [)" << std::endl;
        std::cout << _make_filler<12, ' '>() << nlohmann::json(std::get<0>(syntax)).dump() << "," << std::endl;
        std::cout << _make_filler<12, ' '>() << nlohmann::json(std::get<1>(syntax)).dump() << "," << std::endl;
        std::cout << _make_filler<12, ' '>() << nlohmann::json(std::get<2>(syntax)).dump() << "," << std::endl;
        std::cout << _make_filler<12, ' '>() << nlohmann::json(std::get<3>(syntax)).dump() << std::endl;
        std::cout << _make_filler<8, ' '>() << "]" << std::endl;
        std::cout << _make_filler<4, ' '>() << "}";
    }

    inline void _show_analyzers(const std::vector<std::pair<std::string, std::tuple<sc::list_t, sc::syntax_t>>>& analyzers)
    {
        std::cout << '{' << std::endl;
        _show_analyzer(analyzers[0]);
        for (std::size_t i = 1; i < analyzers.size(); _show_analyzer(analyzers[i++]))
            std::cout << "," << std::endl;
        std::cout << std::endl <<  '}' << std::endl;
    }

    inline void _show_analyzer(const std::string& name)
    {
        if (auto iter = sc::RuleManager::BuildInRules().find(name); iter != sc::RuleManager::BuildInRules().end())
            _show_analyzers({ *iter });
        else
            std::cout << R"(error: No analyzer with name ")" + name + R"(")" << std::endl;
    }

    inline void _show_analyzer()
    {
        _show_analyzers({ sc::RuleManager::BuildInRules().begin(), sc::RuleManager::BuildInRules().end() });
    }

}

#define _opt_keys(k) _help::_get_options(_help::_sc_cmd::k)
#define _opt_key(k) _help::_help_map.at(_help::_sc_cmd::k)._names.front()
