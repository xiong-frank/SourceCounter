English | [中文](./ReadMe.zh-cn.md)

## Source Counter
A source code line counting tool based on C++. It analyzes and count the source code of a specified path, and output the number of files, physical lines, code lines, comment lines and blank lines in different languages.

##### Version: 1.0.0

### Installation
* The command line executable programs for multiple platforms are provided in the release page. You can download them to the local computer for decompression and run them in the terminal.
* You can also choose to build your own. We provide the project settings and scripts for different platforms, refer to [How to build](#How-to-build).

### How to use
The easiest way:
Input in the terminal: `sc [path]` to count the source codes of all supported languages in the `path` and output the results. The `path` parameter can be a directory or a file. For example:
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
```text
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
* **--config, -c**: the statistics tool needs to use the syntax rules of the corresponding language when analyzing the number of lines in the source code. For example, How to identify comment and string? Therefore, you need to provide a configuration file to describe some of the syntax features of these languages. use this option to specify the file path. The configuration file is in the format of `JSON`. For example:
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
    * `syntax` specifies the corresponding syntax rules, including four groups of syntax features, respectively corresponding to: single line comment symbol, multi-line comment symbol, ordinary string symbol and raw string symbol. If some features are not available, it can be configured as empty `[]`.
  * Statistical tools implemented multi language analyzers to analyze and count different languages, and will continue to update to support more language. You can type: `sc --analyzer` to view the supported analyzers. If you want to count a language similar to the syntax of the supported analyzers, you can specify the matching analyzer name in the `analyzer` field of the corresponding configuration item. If not, you can commit a issue to me for, and I will decide whether to implement it after evaluation. for example, the statistical tool is built with the parser of the `Java` language, assuming that you need to count the number of source code lines called `MyJava` language, and its syntax rules are similar to those of `Java` language. Then you can configure as follows:
    ```json
    {
        "MyJava": {
            "analyzer": "Java",
            "extensions": [".javaxx"]
        }
    }
    ```
    Then the statistics tool will recognize the file with the extension of `.javaxx`, and use the `Java` language rule configuration and statistics algorithm to output the `MyJava` language statistics results. If the syntax of the two languages is the same, but the rule configuration is not the same, for example, `Java` uses `/*` `*/` to represent multiline comments, and your `MyJava` language uses `xxx` `yyy`, then you can customize the configuration rules:
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
    `syntax` is optional for non built-in languages. If it is not specified, the corresponding configuration of `analyzer` will be used by default, but the `extensions` field is required. Otherwise, the statistical tool don't know which file should be recognized as `MyJava`, but again, you can still specify the `extensions` field as `[".java"]`, which will give priority to `MyJava` instead of `Java`. Note: language names and analyzer names in the configuration are not case sensitive.
* **--mode, -m**：Even if the grammar rules for different languages are specified, the number of lines may not be determined accurately. This is because even the same code, different people will have different understandings. For example, the following C++ code example:
  ```c++
  int main(/* void */)
  {
       /* to do
      
        */
       return 0;
  }
  ```
  There is no doubt that the number of physical lines in this code is only 7. However, the interpretation of other types of lines seems a bit vague, because there is no uniform standard. For example, in the first line, valid code and comments exist at the same time, some people think that this is both a code line and a comment line, but some people think that one line of data cannot be counted into two lines, so it is only counted as a code line. The blank line under the comment `/* to do` is more likely to cause controversy. Is it a comment or a blank line? And we ca n’t be sure, or we ca n’t decide for the user. Therefore, at this time, you can use the `--mode` option. It is up to the user to decide how ambiguous lines should be interpreted. The ambiguous lines have the following three situations:
  * Case1: One line contains valid code and comments.
  * Case2: Blank line included in a multi-line comment.
  * Case3: A blank line contained in a multi-line string.
  
  We use different values to express specific interpretations in different situations. The following is an explanation of each value:
  
   | name | value | explain |
   | --- | --- | --- |
   | cc_is_code    | 1  | Case1 is Code Line |
   | cc_is_comment | 2  | Case1 is Comment Line |
   | mc_is_blank   | 4  | Case2 is Blank Line |
   | mc_is_comment | 8  | Case2 is Comment Line |
   | ms_is_blank   | 16 | Case3 is Blank Line |
   | ms_is_code    | 32 | Case3 is Code Line |
   
   * It should be noted that different values can be used in combination of **bit-or (or +)** operations, for example: `--mode = 7` and 7 = 1 + 2 + 4, which means: when 7 is specified , Then the **line contains both valid code and comments, which is counted as both a code line and a comment line. And blank lines in multi-line comments are counted as blank lines.** Take the above C++ code as an example. Specify `--mode = 7`, then the statistical result is: *Lines: 7, Codes: 4, Comments: 3, Blanks: 1*.
   * The `--mode` parameter is optional. If not specified, the default value is 39(1+2+4+32).
* **--languages, -l, --exclude, -e**：The language corresponding to the built-in analyzer of the statistical tool and the language in the configuration file will be supported, which usually leads to a lot of languages being supported. I am exactly what we want, but users may not want to analyze all languages when statistics source code, and only want to obtain the analysis and statistics results of some languages he pays attention to. At this time, you can use the `--languages` specification, for example: `--languages​​=C,Java,Python` means statistical tools only the statistical results of the source code files of C, Java, and Python are analyzed, provided that the statistical tools also support these three languages. The `--exclude` option is used to exclude files. Regular expressions are used to match the absolute path of each file. All matching files will not be counted.
* **--thread, -t**: This option is recommended the number of threads should use when the tool is running. Why is it recommended? Because if you specify a large number, of course the statistics program will not allocate so many threads. Generally specify a number less than 32 and not more than the number of target files. It is recommended to take effect.
* **--explain, -x**: Once this option is specified, the statistical tool will not actually perform the statistical work, and only interpret and output the parameter information currently used, provided that all options and parameters are specified correctly. For example, if you want to check whether the input options or parameters are the same as expected before running, you can add this option to view. For example:
  ```shell
  $> ./sc --explain --input resources/sources/ --output report.json --view=lines:des --languages "C,Java,C++,Frank" --exclude="\.c$" --mode=41
  
          input: resources/sources/
         output: report.json
    config file:
      languages: C,Java,C++
      exclusion: \.c$
    allow empty: true
          files: 4
   thread count: 1
           mode: 41(1+8+32)
           view: lines:descending
  ```

### How to build
* Dependent third-party libraries. These libraries are single header file dependencies and have been copied into our project:

  | Name | Version | Description |
  | --- | --- | --- |
  | [NielsLohmann/json](https://github.com/nlohmann/json) | 3.7.3 | Json  library |
  | [FrankXiong/CommandLineParser](https://github.com/xf-bnb/CommandLineParser) | 1.0.3 | Command line parameter parsing library |
  | [FrankXiong/SimpleTest](https://github.com/xf-bnb/SimpleTest) | 1.0.1 | Simple testing framework |
  | [FrankXiong/ConsoleLog](https://github.com/xf-bnb/ConsoleLog) | 1.0.0 | Simple log library |
* Except for the external libraries that it depends on, the project code is completely written in `C++17`. In theory, compilers that support `C++17` can be compiled. However, due to the limitation of the development environment, currently only some machines and compilers have passed the test, the following is the verified compilation environment:

  | Platform | Compiler |
  | --- | --- |
  | Windows 1903 | VisualStudio2019 16.4 |
  | Ubuntu 18.04 | GCC 9.1, Clang 9.0 |
  | MacOS 10.15.2 | GCC 9.2, Clang 9.0, Xcode 11.3 |
* In the [./projects](./projects) directory, the supported build project configurations or scripts are provided for each platform and compiler. Open the configuration directory of the corresponding environment and execute the corresponding script or run the build. The successful build will be output to `./outputs` directory:
  * **Windows:** requires `VisualStudio2017 15.7` or above.
    * Use VisualStudio to open [./projects/Windows/VisualStudio/SourceCounter.sln](./projects/Windows/VisualStudio) project file, and select build from the menu.
  * **Linux:** requires `GCC 9` / `Clang 9` or above.
    * **GCC/Clang:** Go to the corresponding directory in [./projects/Linux](./projects/Linux), and run the `release.sh` / `debug.sh` script.
  * **Mac:** requires `GCC 9` / `Clang 9` / `Xcode 11` or above. Note: For `Clang` or` Xcode` compilation, MacOS version 10.15 or above is required.
    * **Xcode:** Use `Xcode` to load [./projects/Mac/Xcode/SourceCounter.xcworkspace](./projects/Mac/Xcode) project, and build each module separately.
    * **GCC/Clang:** Go to the corresponding directory in [./projects/Mac](./projects/Mac), and run the `release.sh` / `debug.sh` script.
  * Reference:
    * [C++ compiler support](https://en.cppreference.com/w/cpp/compiler_support)
    * [Microsoft C++ language conformance table](https://docs.microsoft.com/zh-cn/cpp/overview/visual-cpp-language-conformance?view=vs-2019)
    * [GCC 9 Release Notes](https://gcc.gnu.org/gcc-9/changes.html)
    * [Clang 9.0.0 Release Notes](https://releases.llvm.org/9.0.0/tools/clang/docs/ReleaseNotes.html)
