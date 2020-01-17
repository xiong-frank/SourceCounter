[English](./ReadMe.md) | 中文

## 源代码行数统计工具
一个基于C++实现的源代码行数统计工具，对指定路径的源代码进行分析统计，输出不同语言的文件数，物理行数，代码行数，注释行数，以及空白行数。

##### Version: 1.0.0

### 安装
* 发布页面中提供了多个平台的命令行可执行程序，下载到本地进行解压，在终端中运行即可。
* 也可以选择自己构建，我们提供了不同平台的构建方式，参考[如何构建](#如何构建)。

### 如何使用
最简单的方式：
在终端中输入：`sc [path]` 即可对 `path` 路径中所有支持的语言的源代码进行统计并输出结果，`path` 参数可以是一个目录，也可以是单个文件路径。示例如下：
```shell
$> ./sc demo.c
+------------+------------+------------+------------+------------+
|      Files |      Lines |      Codes |   Comments |     Blanks |
+------------+------------+------------+------------+------------+
|          1 |         27 |         10 |         10 |          7 |
+------------+------------+------------+------------+------------+
```
统计工具支持多个命令行选项，以决定统计的行为和输出的方式，因此，也可以通过指定多个命令行选项来运行该工具。以下示例统计`demo.c`源文件的行数，并将统计的详细结果输出到`report.json`文件中，统计行数时使用规则值63：
```shell
$> ./sc --input demo.c --output report.json --mode=63
+------------+------------+------------+------------+------------+
|      Files |      Lines |      Codes |   Comments |     Blanks |
+------------+------------+------------+------------+------------+
|          1 |         27 |         10 |         18 |          8 |
+------------+------------+------------+------------+------------+
```
注意：通过指定多个命令行参数来运行统计工具是推荐的做法，可以更加明确的决定统计行为。使用简写方式运行仅在只有一个或两个参数时有效，且两个参数都表示路径，例如：
* `sc demo.c` 等价于 `sc --input demo.c`。
* `sc demo.c report.json` 等价于 `sc --input demo.c --output report.json`。
* 其他情形则必须明确以 `sc [command] [param] ...` 方式执行。

### 支持的命令行参数
```text
     --help, -h  获取帮助信息。
  --version, -v  查看版本信息。
 --analyzer, -a  查看内置的分析器信息。
    --input, -i  [必需] 指定要统计的源代码所在的路径，可以是一个目录路径，也可以是单个文件路径。
   --output, -o  [可选] 指定统计结果输出的位置，若指定，则以json格式输出每个源代码文件的统计详情，否则不输出。
   --config, -c  [可选] 指定不同语言语法规则的配置文件路径。
     --mode, -m  [可选] 指定针对具有歧义的行进行解释的方式。
--languages, -l  [可选] 指定要统计的语言，若不指定则统计所有内置支持的语言。
  --exclude, -e  [可选] 指定要排除的文件，按正则表达式匹配。
   --thread, -t  [可选] 指定建议程序使用的线程数，但并不一定采纳。
  --explain, -x  [可选] 在其他所有命令行参数都正确的情况下，对参数值进行解释输出。
         --view  [可选] 指定终端中输出按语言分类的统计详情以及排序规则。
        --empty  [可选] 指定是否允许空文件。
```

#### 命令行选项释义
* **--ouput, -o**：默认统计工具仅在屏幕输出汇总后的统计结果，要想获得每个文件的统计结果或者意图将结果输出到文件以供其他地方使用，那么需要利用该选项指定输出的文件位置，否则将不输出。
* **--config, -c**：统计工具在分析源代码行数的时候会需要用到对应语言的语法规则，例如对注释和字符串如何判定？因此需要提供一个配置文件来描述这些语言的部分语法特征，使用该选项指定配置文件路径。配置文件采用 `json` 格式，示例配置如下：
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
  * 每个语言的配置项包含3个字段：
    * `analyzer` 指定使用哪种分析器进行分析。
    * `extensions` 指定语言对应文件关联的扩展名。
    * `syntax` 指定对应的语法规则，包括4组语法特征，分别对应：单行注释符号，多行注释符号，普通字符串符号，原生字符串符号，如果某些特征没有，则可以配置为空 `[]`。
  * 统计工具实现了多种语言的分析器，用以对不同语言进行分析统计，并且将会持续迭代以支持更多语言的统计。可以键入：`sc --analyzer` 查看支持的分析器，如果你想统计的语言与支持的分析器的语法类似，则可以在对应配置项的 `analyzer` 字段指定匹配的分析器名称。如果没有，则可以向我提issue，我将在评估之后决定是否实现它。例如，统计工具内置了`Java` 语言的分析器，假定你需要统计一种名叫 `MyJava` 语言的源代码行数，它的语法规则与 `Java` 语言类似。那么你可以进行如下配置：
    ```json
    {
        "MyJava": {
            "analyzer": "Java",
            "extensions": [".javaxx"]
        }
    }
    ```
    那么统计工具将会识别扩展名为 `.javaxx` 的文件，并以 `Java` 语言规则配置和统计算法，输出 `MyJava` 语言的统计结果。如果两种语言的语法一致，但规则配置不一致，例如 `Java` 使用 `/*` `*/` 来表示多行注释，而你的 `MyJava` 语言使用 `xxx` `yyy`，那么也可以自定义配置规则：
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
    `syntax` 对于非内置语言是可选的，不指定时默认使用对应的 `analyzer` 的配置，但 `extensions` 字段必填，否则统计程序不能确定该将哪种文件识别为 `MyJava` 语言，但同时你依然可以指定 `extensions` 字段为 `[".java"]` ，这会优先将 `.java` 文件识别为 `MyJava`，而不是 `Java`。注意：配置中的语言名称和分析器名称不区分大小写。
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
   
   * 需要注意的是，不同的值可以通过 **位或(或加法)** 运算组合使用，例如：`--mode=7` 而 7 = 1 + 2 + 4， 这意味着：当指定7，那么 **一行中同时包含有效代码和注释，该行既统计为代码行，又统计为注释行。并且多行注释中的空行将统计为空行。** 以上述C++代码为例。指定 `--mode=7`，那么统计结果为： *Lines:7, Codes:4, Comments:3, Blanks:1*。
   * `--mode` 参数是可选的，若不指定，默认值为：39(1+2+4+32)。
* **--languages, -l, --exclude, -e**：统计工具内置分析器对应的语言以及配置文件中配置的语言都将得到支持，这通常会导致大量的语言都得到支持，我正是我们想要的，但使用者在统计的时候可能并不想分析所有的源代码，而仅想获得自己关注的某些语言的分析统计结果，此时就可以使用 `--languages` 选项指定，例如：`--languages=C,Java,Python` 意味着统计工具仅分析 C, Java, Python 这3种语言源代码文件的统计结果，前提是统计工具也支持这3种语言。而 `--exclude` 选项则用来排除文件，利用正则表达式对每个文件的绝对路径进行匹配，所有匹配到的文件将不会被统计。
* **--thread, -t**：此选项建议统计工具运行时使用的线程数量，为什么是建议？因为如果指定一个较大的数字，统计程序当然不会同时分配如此多的线程。一般指定一个小于32的数字，且不大于目标文件的数量，建议生效。
* **--explain, -x**：一旦指定该选项，统计工具将不会真正执行统计工作，仅对当前使用的参数信息进行解释输出，前提是所有的选项和参数都指定正确。例如想在真正运行前查看输入的选项或参数是否和预期的一致，可以添加该选项查看。例如：
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
* 在 [./projects](./projects) 目录中按平台和编译器分别提供了支持的构建工程配置或脚本，打开对应环境的配置目录，执行对应的构建脚本或运行构建即可，构建成功将在 `./outputs` 目录中输出生成的结果：
  * **Windows:** 要求 `VS 2017 15.7` 及以上版本。
    * 使用 VisualStudio 打开 [./projects/Windows/VisualStudio/SourceCounter.sln](./projects/Windows/VisualStudio) 工程文件，在菜单中选择构建生成即可。
  * **Linux:** 要求 `GCC 9` / `Clang 9` 及以上版本。
    * **GCC/Clang:** 分别进入 [./projects/Linux](./projects/Linux) 中对应的目录，执行对应环境的 `release.sh` / `debug.sh` 脚本即可。
  * **Mac:** 要求 `GCC 9` / `Clang 9` / `Xcode 11` 及以上版本。注意：对于 `Clang` / `Xcode` 编译，同时要求 `MacOS 10.15` 以上版本。
    * **Xcode:** 使用 `Xcode` 加载 [./projects/Mac/Xcode/SourceCounter.xcworkspace](./projects/Mac/Xcode) 工程，分别对每个模块进行构建即可。
    * **GCC/Clang:** 分别进入 [./projects/Mac](./projects/Mac) 中对应的目录，执行对应环境的 `release.sh` / `debug.sh` 脚本即可。
  * 参考：
    * [C++ 编译器支持情况](https://en.cppreference.com/w/cpp/compiler_support)
    * [Microsoft C++ 语言一致性](https://docs.microsoft.com/zh-cn/cpp/overview/visual-cpp-language-conformance?view=vs-2019)
    * [GCC 9 发行说明](https://gcc.gnu.org/gcc-9/changes.html)
    * [Clang 9.0.0 发行说明](https://releases.llvm.org/9.0.0/tools/clang/docs/ReleaseNotes.html)
