## 源代码行数统计工具
一个基于C++实现的源代码行数统计工具，对指定路径的源代码进行分析统计，输出不同语言的文件数，物理行数，代码行数，注释行数，以及空白行数。

##### Version: 1.0.0-SNAPSHOT

### Usage
最简单的方式：
在终端中输入：`SourceCounter [path]` 即可对 `path` 路径中所有支持的语言的源代码进行统计并输出结果，`path`  参数可以是一个目录，也可以是单个文件路径。示例如下：
```shell
$> ./SourceCounter demo.c
+------------+------------+------------+------------+------------+
|      Files |      Lines |      Codes |   Comments |     Blanks |
+------------+------------+------------+------------+------------+
|          1 |         27 |         10 |         10 |          7 |
+------------+------------+------------+------------+------------+
```
统计工具支持多个命令行选项，以决定统计的行为和输出的方式，因此，也可以通过指定多个命令行选项来运行该工具，例如：
```shell
$> ./SourceCounter --input demo.c --mode=63
+------------+------------+------------+------------+------------+
|      Files |      Lines |      Codes |   Comments |     Blanks |
+------------+------------+------------+------------+------------+
|          1 |         27 |         10 |         18 |          8 |
+------------+------------+------------+------------+------------+
```

#### 支持的命令行参数
```text
     --help, -h  获取帮助信息。
  --version, -v  输出版本信息。
    --input, -i  [必需] 指定要统计的源代码所在的路径，可以是一个目录路径，也可以是单个文件路径。
   --output, -o  [可选] 指定统计结果输出的位置，若指定，则以json格式输出每个源代码文件的统计详情，否则不输出。
   --config, -c  [可选] 指定不同语言语法规则的配置文件路径。
     --mode, -m  [可选] 指定针对具有歧义的行进行解释的方式。
--languages, -l  [可选] 指定要统计的语言，若不指定则统计所有内置支持的语言。
  --exclude, -e  [可选] 指定要排除的文件，按正则表达式匹配。
   --detail, -d  [可选] 指定终端中输出按语言分类的统计详情以及排序规则。
   --thread, -t  [可选] 指定建议程序使用的线程数，但并不一定采纳。
  --explain, -x  [可选] 在其他所有命令行参数都正确的情况下，对参数值进行解释输出。
        --empty  [可选] 指定是否允许空文件。
```

#### 命令选项释义
* **--ouput, -o**：默认统计工具仅在屏幕输出汇总后的统计结果，要想获得每个文件的统计结果或者意图将结果输出到文件以供其他地方使用，那么需要利用该选项指定输出的文件位置，否则将不输出。
* **--config, -c**：统计工具在分析源代码行数的时候会需要用到对应语言的语法规则，例如对注释和字符串如何判定？因此需要提供一个配置文件来描述这些语言的部分语法特征，使用该选项指定配置文件路径。配置文件采用 `json` 格式，示例配置如下：
  ```json
  {
      "C++": {
          "extensions": [".cpp", ".cc", ".inl"],
          "options": [ ["//"],
                       [["/*", "*/"]],
                       [["\"", "\""]],
                       [["R\"(", ")\""]] 
                     ]
      },
      "Java": {
          "extensions": [".java"],
          "options": [ ["//"],
                       [["/*", "*/"]],
                       [["\"", "\""]],
                       [] 
                     ]
      }
  }
  ```
  * 其中 `options` 字段可能需要配置4组语法特征，分别对应：单行注释符号，多行注释符号，普通字符串符号，原生字符串符号，如果某些特征没有，则可以不配置。
  * 统计工具默认会内置一些语言的语法规则(要查看内置支持的语言列表可以键入：`SourceCounter -x -i .`)。内置支持的语言配置规则不允许修改，即使配置文件中配置了也会忽略，但可以配置其对应的文件扩展名。
* **--mode, -m**：即使指定了不同语言的语法规则，也不一定就能准确的对行数进行判定，这是因为，即便是相同代码，不同的人会有不同的理解，例如下面的 C++ 代码示例：
  ```C++
  int main(/* void */)
  {
      /* to do
      
       */
       return 0;
  }
  ```
  这段代码的物理行数一共只有7行，这毋庸置疑。但对于其他类型的行的释义则显得有些模糊，因为没有一个统一的标准。例如第一行中，有效代码和注释同时存在，有的人认为这既是代码行也是注释行，但有的人认为一行数据不能统计成两行，因此仅算作代码行。而注释 `/* to do` 下面的空白行则更容易引起争论，到底算注释还是空行？而我们也不能确定，或者说不能替使用者决定。因此，此时就可以使用 `--mode` 选项，由使用者来自己来决定具有歧义的行应该如何解释，具有歧义的行有下面3种情况：
  * Case1: 多行注释中包含的空白行。
  * Case2: 多行字符串中包含的空白行。
  * Case3: 一行中同时包含有效代码和注释。
  
  我们采用不同的值来表达不同情况下的具体释义，下面是每种值的解释：
  
   | name | value | explain |
   | --- | --- | --- |
   | mc_is_blank   | 1  | Case1 is Blank Line |
   | mc_is_comment | 2  | Case1 is Comment Line |
   | ms_is_code    | 4  | Case2 is Code Line |
   | ms_is_blank   | 8  | Case2 is Blank Line |
   | cc_is_code    | 16 | Case3 is Code Line |
   | cc_is_comment | 32 | Case3 is Comment Line |
   
   需要注意的是，不同的值可以通过 **位或** (或加法)运算组合使用，例如：`--mode=48` 而 48 = 16 + 32 这意味着指定48，那么当 **一行中同时包含有效代码和注释** 的时候，该行 **既统计为代码行，又统计为注释行。**
* **--languages, -l, --exclude, -e**：统计工具内置的语言以及配置文件中配置的语言都将得到支持，这通常会导致大量的语言都得到支持，我正是我们想要的，但使用者在统计的时候可能并不想分析所有的源代码，而仅想获得自己关注的某些语言的分析统计结果，此时就可以使用 `--languages` 选项指定，例如：`--languages=C,Java,Python` 意味着统计工具仅分析 C, Java, Python 这3种语言源代码文件的统计结果，前提是统计工具也支持这3种语言。而 `--exclude` 选项则用来排除文件，利用正则表达式对每个文件的绝对路径进行匹配，所有匹配到的文件将不会被统计。
* **--thread, -t** 此选项建议统计工具运行时使用的线程数量，为什么是建议？因为如果指定一个较大的数字，统计程序当然不会同时分配如此多的线程。一般指定一个小于32的数字，且不大于目标文件的数量，建议生效。
* **--explain, -x** 一旦指定该选项，统计工具将不会真正执行统计工作，仅对当前使用的参数信息进行解释输出，前提是所有的选项和参数都指定正确。例如想在真正运行前查看输入的选项或参数是否和预期的一致，可以添加该选项查看。