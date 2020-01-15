#pragma once

#include "../sc-counter/Counter.h"

namespace sc
{
    // 定义报告显示规则
    enum order_t
    {
        // 定义排序列
        no_show         = 0x00,
        by_nothing      = 0x01,
        by_lines        = 0x02,
        by_codes        = 0x03,
        by_comments     = 0x04,
        by_blanks       = 0x05,
        by_files        = 0x06,
        order_mask      = 0x07,

        // 定义排序方向
        ascending       = 0x00,
        descending      = 0x08,
        order_direction = 0x08
    };

    struct params_t
    {
        std::vector<std::string> languages;
        std::string input;
        std::string output;
        std::string configFile;
        std::string exclusion;
        unsigned int nThread{ 0 };
        unsigned int view{ 0 };
        unsigned int mode{ mode_t::default_mode };
        bool allowEmpty{ true };

    };  // struct params_t

    /*
     * 从命令行参数解析配置
     * 将解析到的数据保存到 params_t 数据结构中
     * 若解析完成后允许对源代码进行分析统计则返回 true，否则返回 false。
     */
    bool ParseCommandLine(Counter& counter, params_t& opt, const char* const* argv, unsigned int argc);

    // 输出统计报告结果
    void OutputReport(const std::vector<Counter::file_report_t>& reports, const std::string& filename, unsigned int view);

}
