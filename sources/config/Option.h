#ifndef _Option_H_
#define _Option_H_

namespace sc
{


    class Option final
    {
        using string_type = std::string;

        template<typename _Type>
        using list_type = std::vector<_Type>;

        list_type<string_type> languages;
        string_type input;
        string_type output;
        string_type config_file;
        string_type exclusions;
        unsigned int detail;
        unsigned int mode;

        Option() = default;
        Option(const Option&) = delete;
        Option& operator = (const Option&) = delete;

    public:

        static Option& GetInstance() { static Option _opt; return (_opt); }

        static bool ParseCommandLine(const char* const argv[]);
        
    };  // class Option

#define _opt_ sc::Option::GetInstance()
}

#endif // !_Option_H_
