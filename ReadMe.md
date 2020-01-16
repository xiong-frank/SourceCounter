English | [中文](./ReadMe.zh-cn.md)

## Source Counter
A source code line counting tool based on C++. It analyzes and count the source code of a specified path, and output the number of files, physical lines, code lines, comment lines and blank lines in different languages.

##### Version: 1.0.0-SNAPSHOT

### Installation
* The command line executable programs for multiple platforms are provided in the release page. You can download them to the local computer for decompression and run them in the terminal.
* You can also choose to build your own. We provide the project settings and scripts for different platforms, refer to [How to build](#How to build).

### How to use
The easiest way:
Input in the terminal: 'sc [path]' to count the source codes of all supported languages in the 'path' and output the results. The 'path' parameter can be a directory or a file. For example:
```shell
$> ./sc demo.c
+------------+------------+------------+------------+------------+
|      Files |      Lines |      Codes |   Comments |     Blanks |
+------------+------------+------------+------------+------------+
|          1 |         27 |         10 |         10 |          7 |
+------------+------------+------------+------------+------------+
```
The statistics tool supports multiple command-line options to determine how the statistics behave and output, so you can also run the tool by specifying multiple command-line options. The following example counts the number of lines of the `demo.c` source file, and outputs the detailed results of the statistics to the `report.json`, and use mode value 63:
```shell
$> ./sc --input demo.c --output report.json --mode=63
+------------+------------+------------+------------+------------+
|      Files |      Lines |      Codes |   Comments |     Blanks |
+------------+------------+------------+------------+------------+
|          1 |         27 |         10 |         18 |          8 |
+------------+------------+------------+------------+------------+
```
Note: It is recommended to run the statistics tool by specifying multiple command line parameters, which can more clearly determine the statistical behavior. Running in short form works only when there are only one or two parameters, and both parameters represent the path, for example:
* `sc demo.c` is equivalent to `sc --input demo.c`.
* `sc demo.c report.json` is equivalent to `sc --input demo.c --output report.json`.
* In other cases, it must be explicitly executed as `sc [command] [param] ...`.

### Supported command line parameters
```shell
      --help,-h  For help information.
   --version,-v  Show version information.
  --analyzer,-a  Show built-in analyzer information.
     --input,-i  [required] Specify the path of the source to be counted, can be a file or directory.
    --output,-o  [optional] Specify the file path for the output statistics, the output is in JSON format.
    --config,-c  [optional] Specifies the config path for language syntax rules.
      --mode,-m  [optional] Specify statistical rules to explain the ambiguous lines.
 --languages,-l  [optional] Specify the language for Statistics.
   --exclude,-e  [optional] A regular expression, specify paths to exclude.
    --thread,-t  [optional] suggested number of threads used.
   --explain,-x  [optional] explain parameters to be used during program execution.
         --view  [optional] Show statistics by language, and you can also specify sorting rules.
        --empty  [optional] Specifies whether to count empty files. default: true.
```

#### Command line option definition
* **--ouput, -o**: the default statistics tool only outputs the summarized statistics results on the console. In order to obtain the statistics results of each file or output the results to the file for other uses, you need to use this option to specify the output file location, otherwise it will not output.

* **--config, -c**: the statistics tool needs to use the syntax rules of the corresponding language when analyzing the number of lines in the source code. For example, how to determine comments and strings? Therefore, you need to provide a configuration file to describe some of the syntax features of these languages. use this option to specify the file path. The configuration file is in the format of 'JSON'. For example:
  ```json
  {
      "C++": {
          "extensions": [".cpp", ".hpp", ".inl"],
          "syntax": [ ["//"],
                      [["/*", "*/"]],
                      [["\"", "\""]],
                      [["R\"(", ")\""]] 
                    ]
      },
      "language-name": {
          "analyzer": "Java",
          "extensions": [".java"],
          "syntax": [ ["//"],
                      [["/*", "*/"]],
                      [["\"", "\""]],
                      [] 
                    ]
      }
  }
  ```
  * Configuration items for each language contain three fields:
    * `analyzer` specifies which analyzer to use for statistics.
    * `extensions` specifies the extension associated with the file corresponding to the language.
    * `syntax` specifies the corresponding syntax rules, including four groups of syntax features, respectively corresponding to: single line comment symbol, multi line comment symbol, common string symbol and raw string symbol. If some features are not available, it can be configured as empty '[]'.
  * Statistical tools implemented multi language analyzers to analyze and count different languages, and will continue to update to support more language. You can type: `sc --analyzer` to view the supported analyzers. If you want to count a language similar to the syntax of the supported analyzers, you can specify the matching analyzer name in the `analyzer` field of the corresponding configuration item. If not, you can commit a issue to me for, and I will decide whether to implement it after evaluation. for example, the statistical tool is built with the parser of the `Java` language, assuming that you need to count the number of source code lines called `MyJava` language, and its syntax rules are similar to those of `Java` language. Then you can configure as follows:
    ```json
    {
        "MyJava": {
            "analyzer": "Java",
            "extensions": [".javaxx"]
        }
    }
    ```
    Then the statistics tool will recognize the file with the extension of `.javaxx`, and use the `Java` language rule configuration and statistics algorithm to output the `Myjava` language statistics results. If the syntax of the two languages is the same, but the rule configuration is not the same, for example, `Java` uses `/*` `*/` to represent multiline comments, and your `Myjava` language uses `xxx` `YYY`, then you can customize the configuration rules:
    ```json
    {
        "MyJava": {
            "analyzer": "Java",
            "extensions": [".javaxx"],
            "syntax": [ ["//"],
                      [["xxx", "yyy"]],
                      [["\"", "\""]],
                      [] 
                    ]
        }
    }
    ```
    `syntax` is optional for non built-in languages. If it is not specified, the corresponding configuration of `analyzer` will be used by default, but the `extensions` field is required. Otherwise, the statistical tool don't know which file should be recognized as `Myjava`, but again, you can still specify the `extensions` field as `[".java"]`, which will give priority to `Myjava` instead of `Java`. Note: language names and analyzer names in the configuration are not case sensitive.
* **--mode, -m**：即使指定了不同语言的语法规则，也不一定就能准确的对行数进行判定，这是因为，即便是相同代码，不同的人会有不同的理解，例如下面的 C++ 代码示例：
  ```c++
  int main(/* void */)
  {
      /* to do
      
       */
       return 0;
  }
  ```
  这段代码的物理行数一共只有7行，这毋庸置疑。但对于其他类型的行的释义则显得有些模糊，因为没有一个统一的标准。例如第一行中，有效代码和注释同时存在，有的人认为这既是代码行也是注释行，但有的人认为一行数据不能统计成两行，因此仅算作代码行。而注释 `/* to do` 下面的空白行则更容易引起争论，到底算注释还是空行？而我们也不能确定，或者说不能替使用者决定。因此，此时就可以使用 `--mode` 选项，由使用者来自己来决定具有歧义的行应该如何解释，具有歧义的行有下面3种情况：
  * Case1: 一行中同时包含有效代码和注释。
  * Case2: 多行注释中包含的空白行。
  * Case3: 多行字符串中包含的空白行。
  
  我们采用不同的值来表达不同情况下的具体释义，下面是每种值的解释：
  
   | name | value | explain |
   | --- | --- | --- |
   | cc_is_code    | 1  | Case1 is Code Line |
   | cc_is_comment | 2  | Case1 is Comment Line |
   | mc_is_blank   | 4  | Case2 is Blank Line |
   | mc_is_comment | 8  | Case2 is Comment Line |
   | ms_is_blank   | 16 | Case3 is Blank Line |
   | ms_is_code    | 32 | Case3 is Code Line |
   
   * 需要注意的是，不同的值可以通过 **位或** (或加法)运算组合使用，例如：`--mode=7` 而 7 = 1 + 2 + 4， 这意味着：当指定7，那么 **一行中同时包含有效代码和注释，该行既统计为代码行，又统计为注释行。并且多行注释中的空行将统计为空行。** 以上述C++代码为例。指定 `--mode=7`，那么统计结果为： *Lines:7, Codes:4, Comments:3, Blanks:1*。
   * `--mode` 参数是可选的，若不指定，默认值为：39(1 + 2 + 4 + 32)。
* **--languages, -l, --exclude, -e**：统计工具内置分析器对应的语言以及配置文件中配置的语言都将得到支持，这通常会导致大量的语言都得到支持，我正是我们想要的，但使用者在统计的时候可能并不想分析所有的源代码，而仅想获得自己关注的某些语言的分析统计结果，此时就可以使用 `--languages` 选项指定，例如：`--languages=C,Java,Python` 意味着统计工具仅分析 C, Java, Python 这3种语言源代码文件的统计结果，前提是统计工具也支持这3种语言。而 `--exclude` 选项则用来排除文件，利用正则表达式对每个文件的绝对路径进行匹配，所有匹配到的文件将不会被统计。
* **--thread, -t**： 此选项建议统计工具运行时使用的线程数量，为什么是建议？因为如果指定一个较大的数字，统计程序当然不会同时分配如此多的线程。一般指定一个小于32的数字，且不大于目标文件的数量，建议生效。
* **--explain, -x**： 一旦指定该选项，统计工具将不会真正执行统计工作，仅对当前使用的参数信息进行解释输出，前提是所有的选项和参数都指定正确。例如想在真正运行前查看输入的选项或参数是否和预期的一致，可以添加该选项查看。例如：
  ```shell
  $> ./sc --explain --input resources/sources/ --output report.json --view=lines:des --languages "C,Java.C++,Frank" --exclude="\.c$" --mode=41
  
          input: resources/sources/
         output: report.json
    config file:
      languages: C
      exclusion: \.c$
    allow empty: true
          files: 2
   thread count: 1
           mode: 41(1+8+32)
           view: lines:descending
  ```

### 如何构建
* 依赖的第三方库：

  | Name | Version | Description |
  | --- | --- | --- |
  | [NielsLohmann/json](https://github.com/nlohmann/json) | 3.7.3 | Json 库 |
  | [FrankXiong/CommandLineParser](https://github.com/xf-bnb/CommandLineParser) | 1.0.3 | 命令行参数解析库 |
  | [FrankXiong/SimpleTest](https://github.com/xf-bnb/SimpleTest) | 1.0.1 | 简单的测试框架 |
  | [FrankXiong/ConsoleLog](https://github.com/xf-bnb/ConsoleLog) | 1.0.0 | 简单的日志框架 |
* 除依赖的外部库以外，项目代码完全采用`C++17`编写，理论上支持`C++17`的编译器均可编译，但受于开发环境限制，目前仅在部分机器和编译器上测试通过，以下是经过验证的编译环境：

  | Platform | Compiler |
  | --- | --- |
  | Windows 1903 | VisualStudio2019 16.4 |
  | Ubuntu 18.04 | GCC 9.1, Clang 9.0 |
  | MacOS 10.15.2 | GCC 9.2, Clang 9.0, Xcode 11.3 |
* 在 [projects](./projects) 目录中按平台和编译器分别提供了支持的构建工程配置或脚本，打开对应环境的配置目录，执行对应的构建脚本或运行构建即可，构建成功将在 `./outputs` 目录中输出生成的结果：
  * **Windows:** 要求 `VS 2017 15.7` 及以上版本。
    * 使用 VisualStudio 打开 [./projects/Windows/VisualStudio/SourceCounter.sln](./projects/Windows/VisualStudio) 工程文件，在菜单中选择构建生成即可。
  * **Linux:** 要求 `GCC 9` 或 `Clang 9` 及以上版本。
    * **GCC/Clang:** 分别进入 [./projects/Linux](./projects/Linux) 中对应的目录，执行对应环境的 `release.sh` / `debug.sh` 脚本即可。
  * **Mac:** 要求 `GCC 9` 或 `Clang 9` 或 `Xcode 11` 及以上版本。注意：对于 `Clang` 或 `Xcode` 编译，同时要求 `MacOS 10.15` 以上版本。
    * **Xcode:** 使用 `Xcode` 加载 [./projects/Mac/Xcode/SourceCounter.xcworkspace](./projects/Mac/Xcode) 工程，分别对每个模块进行构建即可。
    * **GCC/Clang:** 分别进入 [./projects/Mac](./projects/Mac) 中对应的目录，执行对应环境的 `release.sh` / `debug.sh` 脚本即可。
  * 参考：
    * [C++ 编译器支持情况](https://en.cppreference.com/w/cpp/compiler_support)
    * [Microsoft C++ 语言一致性](https://docs.microsoft.com/zh-cn/cpp/overview/visual-cpp-language-conformance?view=vs-2019)
    * [GCC 9 发行说明](https://gcc.gnu.org/gcc-9/changes.html)
    * [Clang 9.0.0 发行说明](https://releases.llvm.org/9.0.0/tools/clang/docs/ReleaseNotes.html)
