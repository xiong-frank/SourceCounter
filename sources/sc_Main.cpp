#include <list>
#include <queue>
#include <iostream>

#include "utils/PathUtils.h"
#include "log/sc_log.h"
#include "config/ConfigInfo.h"
#include "counter/FileReport.h"
#include "counter/ReportList.h"
#include "counter/AnalyzeThread.h"

int main(int argc, char *argv[])
{
    // 校验参数
    if (argc < 2)
    {
        std::cout << "You don\'t specify any path !" << std::endl;
        return 0;
    }

    // 初始化日志
    _log_init(argv[0]);

    // 提取路径参数列表
    std::vector<std::string> dirList;
    for (int i = 1; i < argc; ++i)
        ExtractDirectory(dirList, argv[i]);

    // 在参数列表中没有找到任何有效的路径
    if (dirList.empty())
    {
        std::cout << "All of the paths you have specified are invalid !" << std::endl;
        return 0;
    }

    // 加载配置文件
    ConfigInfo config;
    config.Load(AppointSuffix(argv[0], ".ini").c_str());
    config.Show();

    // 定义分析线程对象
    AnalyzeThread fileThread;

    // 根据指定的文件后缀名从目录中统计源代码文件列表
    for (std::string& dir : dirList)
    {
        unsigned int n = fileThread.ExtractFile(dir, config.GetSuffixList());
        _log("found %d files from \"%s\"", n, dir.c_str());
    }

    // 在所有的参数列表路径中没有找到任何源代码文件
    if (0 == fileThread.GetFileCount())
    {
        std::cout << "All of the paths you have specified hasn\'t found any source file !" << std::endl;
        return 0;
    }

    clock_t t1 = clock();

    // 定义统计报告对象
    ReportList reports(fileThread.GetFileCount());

    // 使用指定配置参数启动计算统计过程
    fileThread.Start(config.GetThreadCount(), reports, config.GetSingleList(), config.GetMultipleList());

    clock_t t2 = clock();

    // 对计算好的统计结果排序
    reports.Sort(
        [](const FileReport& a, const FileReport& b)
        {
            // 默认按文件名排序
            return a.GetFilePath() < b.GetFilePath();
            // 或者按其他方式排序
            // return a.GetFilePath()   > b.GetFilePath();
            // return a.GetTotal()      < b.GetTotal();
            // return a.GetEffective()  < b.GetEffective();
            // return a.GetSpendTime()  < b.GetSpendTime();
        }
    );

    clock_t t3 = clock();

    // 显示统计报告结果
    reports.Show();

    _log("analyze %d files spend: %ums, sort spend: %ums", reports.GetTotalFile(), t2 - t1, t3 - t2);

    return 0;
}
