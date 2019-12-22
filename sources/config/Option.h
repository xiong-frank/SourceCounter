#pragma once

namespace xf::cmd {
    class result_t;
}

namespace sc
{
    enum order_t {
        no_show     = 0x00,
        by_nothing  = 0x01,
        by_lines    = 0x02,
        by_codes    = 0x03,
        by_comments = 0x04,
        by_blanks   = 0x05,
        by_files    = 0x06,
        order_mask  = 0x07,

        ascending   = 0x00,
        descending  = 0x08,
        order_direction  = 0x08
    };

    enum mode_t {
        cc_is_code      = 0x01,
        cc_is_comment   = 0x02,
        mc_is_blank     = 0x04,
        mc_is_comment   = 0x08,
        ms_is_blank     = 0x10,
        ms_is_code      = 0x20
    };

    class Option final
    {
        std::vector<std::string> languages;
        std::string input;
        std::string output;
        std::string configFile;
        std::string exclusion;
        unsigned int nThread{ 0 };
        unsigned int detail{ 0 };
        unsigned int mode{ mode_t::mc_is_blank | mode_t::ms_is_code | mode_t::cc_is_code };
        bool empty{ true };

        Option() = default;
        Option(const Option&) = delete;
        Option& operator = (const Option&) = delete;

        bool _parse_option(const xf::cmd::result_t& result);

    public:

        const std::vector<std::string>& Languages() const { return languages; }
        bool AllowEmpty() const { return empty; }
        bool CheckMode(unsigned int m) const { return ((mode & m) == m); }
        const std::string& ConfigFile() const { return configFile; }
        const std::string& InputPath() const { return input; }
        const std::string& OutputPath() const { return output; }
        const std::string& Exclusion() const { return exclusion; }
        unsigned int ThreadNumber() const { return nThread; }
        unsigned int Detail() const { return detail; }
        void Explain() const;

        static bool ParseCommandLine(const char* const* argv, unsigned int argc);

        static Option& Instance() { static Option _opt; return (_opt); }

    };  // class Option

}   // namespace sc

#define _sc_opt sc::Option::Instance()
