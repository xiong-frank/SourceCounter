#ifndef _Option_H_
#define _Option_H_

namespace sc
{
    enum Mode {
        is_comment
    };

    class Option final
    {
        using string_type = std::string;

        template<typename _Type>
        using list_type = std::vector<_Type>;

        list_type<string_type> languages;
        string_type input;
        string_type output;
        string_type configFile;
        string_type exclusion;
        unsigned int nThread;
        unsigned int detail;
        unsigned int mode;
        bool explain{ false };

        Option() = default;
        Option(const Option&) = delete;
        Option& operator = (const Option&) = delete;

    public:

        bool Explain() const { return explain; }
        const std::string& ConfigFile() const { return configFile; }
        const std::string& InputPath() const { return input; }
        const std::string& OutputPath() const { return output; }
        const std::string& Exclusion() const { return exclusion; }
        unsigned int ThreadNumber() const { return nThread; }
        const list_type<string_type>& Languages() const { return languages; }
        bool CheckMode(unsigned int m) const { return false; }
        unsigned int Detail() const { return detail; }


        static Option& Instance() { static Option _opt; return (_opt); }

        static bool ParseCommandLine(const char* const * argv, std::size_t argc);
        
    };  // class Option

#define _sc_opt sc::Option::Instance()
}

#endif // !_Option_H_
