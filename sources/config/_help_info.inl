
enum _sc_cmd {
    _sc_cmd_help,
    _sc_cmd_version,
    _sc_cmd_input,
    _sc_cmd_output,
    _sc_cmd_config,
    _sc_cmd_mode,
    _sc_cmd_languages,
    _sc_cmd_exclude,
    _sc_cmd_detail,
    _sc_cmd_empty,
    _sc_cmd_thread,
    _sc_cmd_explain
};

const std::map<std::string, unsigned int> _sc_cmd_map{
    {"--help", _sc_cmd::_sc_cmd_help},  {"-h", _sc_cmd::_sc_cmd_help},
    {"--version", _sc_cmd::_sc_cmd_version},  {"-v", _sc_cmd::_sc_cmd_version},
    {"--input", _sc_cmd::_sc_cmd_input},  {"-i", _sc_cmd::_sc_cmd_input},
    {"--output", _sc_cmd::_sc_cmd_output},  {"-o", _sc_cmd::_sc_cmd_output},
    {"--config", _sc_cmd::_sc_cmd_config},  {"-c", _sc_cmd::_sc_cmd_config},
    {"--mode", _sc_cmd::_sc_cmd_mode},  {"-m", _sc_cmd::_sc_cmd_mode},
    {"--languages", _sc_cmd::_sc_cmd_languages},  {"-l", _sc_cmd::_sc_cmd_languages},
    {"--exclude", _sc_cmd::_sc_cmd_exclude},  {"-e", _sc_cmd::_sc_cmd_exclude},
    {"--detail", _sc_cmd::_sc_cmd_detail},  {"-d", _sc_cmd::_sc_cmd_detail},
    {"--empty", _sc_cmd::_sc_cmd_empty},
    {"--thread", _sc_cmd::_sc_cmd_thread},  {"-t", _sc_cmd::_sc_cmd_thread},
    {"--explain", _sc_cmd::_sc_cmd_explain},  {"-x", _sc_cmd::_sc_cmd_explain}
};

const std::map<unsigned int, std::string> _sc_help_map{
{_sc_cmd::_sc_cmd_help,
R"(  --help, -h: 获取帮助信息
    用法：--help [command]
    示例：--help
          --help --input
          --help --config
)"},

{_sc_cmd::_sc_cmd_version,
R"(  --version, -v: 查看版本信息
    用法：--version
    示例：--version
)"},

{_sc_cmd::_sc_cmd_input,
R"(  --input, -i: (必需)指定要进行统计的输入路径，可以是一个文件或者目录。
    用法：--input [path]
    示例：--input /Users/Name/main.cpp
          --input /Users/Name/Sources/
)"},

{_sc_cmd::_sc_cmd_output,
R"(  --output, -o: (可选)指定统计信息输出的文件，输出信息为json格式。
                 包含每个文件的统计结果和汇总的统计结果，若不指定则不输出。
    用法：--output [command]
    示例：--output /Users/Name/sc.json
)"},

{_sc_cmd::_sc_cmd_config,
R"(  --config, -c: (可选)指定不同语言语法规则的配置文件路径
                 若不指定，统计程序仅采用内置规则。
                 文件配置以json格式，示例：
                [
                   {"name": "Python",
                    }
                ]
    用法：--config [path]
    示例：--config /Users/Name/languages.json
)"},

{_sc_cmd::_sc_cmd_mode,
R"(  --mode, -m: (可选)指定统计规则，仅针对具有歧义的行进行解释。
               具有歧义的行是指一行中包含多种属性的情况，例如：
               一行中同时有代码和注释，或者多行注释中的空行，以及多行字符串中的空行。
               针对具有歧义的行指定统计规则以得到明确的统计结果，规则值如下：

               name           value  exlpain
               mc_is_blank    1      对于多行注释中的空行，此行算作空白行。
               mc_is_comment  2      对于多行注释中的空行，此行算作注释行。
               ms_is_code     4      对于跨行字符串中的空行，此行算作代码行。
               ms_is_blank    8      对于跨行字符串中的空行，此行算作空白行。
               cc_is_code     16     对于同时包含代码和注释的行，此行算作代码行。
               cc_is_comment  32     对于同时包含代码和注释的行，此行算作注释行。

               不同规则可以组合使用，按其值的位或运算(或加法运算)得到的结果作为"--mode"参数的值。
               例如：若要将多行注释中的空行算作空白行，跨行字符串中的空行算作代码行，
                     同时包含注释和代码的行，既算作代码行也算作注释行。
               可以给"--mode"参数指定值:(1|4|16|32) = 53。即："--mode=53"
    用法：--mode [number]
    示例：--mode 7
)"},

{_sc_cmd::_sc_cmd_languages,
R"(  --languages, -l: (可选)指定统计的语言
                    可指定的语言包括内置语言和配置文件中配置的语言。
                    多个语言之间使用逗号分隔，语言名称不区分大小写。
    用法：--languages [name list]
    示例：--languages C++,Java,Python
)"},

{_sc_cmd::_sc_cmd_exclude,
R"(  --exclude, -e: (可选)一个正则表达式字符串，指定要排除的路径。
                  对于匹配该正则表达式的文件路径，将不会进行统计。
    用法：--exclude [regular expression]
    示例：--exclude *.h
)"},

{_sc_cmd::_sc_cmd_detail,
R"(  --detail, -d: (可选)按语言分类在终端中展示统计结果，可以同时指定排序规则。
                 排序列可以指定：files, lines, codes, comments, blanks
                 排序方式可以指定：(升序)asc[ending], (降序)des[cending]
    用法：--detail [command]
    示例：--detail
          --detail codes|asc
)"},

{_sc_cmd::_sc_cmd_empty,
R"(  --empty: (可选)指定是否统计空文件，默认为：true。
    用法：--empty [bool]
    示例：--empty true
)"},

{_sc_cmd::_sc_cmd_thread,
R"(  --thread, -t: (可选)建议使用的线程数量
                 统计程序本身会根据识别到的文件数量自动调整线程数。
                 如果指定的线程数太大将忽略指定的值。
    用法：--thread [number]
    示例：--thread 4
)"},

{_sc_cmd::_sc_cmd_explain,
R"(  --explain, -x: (可选)若指定该选项，则不进行实际的行数统计，仅对当前程序准备统计使用的参数进行解释。
    用法：--explain
    示例：--explain
)"}
};

inline const char* version() { return "1.0.0-snapshot"; }
inline const char* app_name() { return "SourceCounter"; }

inline void _show_help(const std::string& k)
{
    std::cout << _sc_help_map.at(_sc_cmd_map.at(k)) << std::endl;
}
