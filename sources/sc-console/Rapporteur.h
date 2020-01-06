#pragma once

#include "../sc-counter/Counter.h"

namespace sc
{
    // 定义报告显示规则
    enum order_t
    {
        no_show         = 0x00,
        by_nothing      = 0x01,
        by_lines        = 0x02,
        by_codes        = 0x03,
        by_comments     = 0x04,
        by_blanks       = 0x05,
        by_files        = 0x06,
        order_mask      = 0x07,

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
        unsigned int detail{ 0 };
        unsigned int mode{ mode_t::default_mode };
        bool allowEmpty{ true };

    };  // struct params_t

    // 从命令行参数解析配置
    bool ParseCommandLine(Counter& counter, params_t& opt, const char* const* argv, unsigned int argc);

    // 对报告结果进行输出
    void OutputReport(const std::vector<Counter::file_report_t>& reports, const std::string& filename, unsigned int detail);

}
