#pragma once

namespace sc
{
    /*
    --explain
    --languages=c++,java
    --input=/home/dir
    --output=/home/file
    --config=/home/config.json
    --mode=123
    --help
    --version
    --exclude=*.h
    --detail=lines|asc
    --thread=10

    */

    enum Mode {
        is_comment
    };

    class Option final
    {
        std::vector<std::string> languages;
        std::string input;
        std::string output;
        std::string configFile;
        std::string exclusion;
        unsigned int nThread;
        unsigned int detail;
        unsigned int mode;
        bool explain{ false };
        bool empty{ false };

        Option() = default;
        Option(const Option&) = delete;
        Option& operator = (const Option&) = delete;

    public:

        bool AllowEmpty() const { return empty; }
        bool Explaining() const { return explain; }
        const std::string& ConfigFile() const { return configFile; }
        const std::string& InputPath() const { return input; }
        const std::string& OutputPath() const { return output; }
        const std::string& Exclusion() const { return exclusion; }
        unsigned int ThreadNumber() const { return nThread; }
        const std::vector<std::string>& Languages() const { return languages; }
        bool CheckMode(unsigned int m) const { return false; }
        unsigned int Detail() const { return detail; }

        void Explain(const std::vector<std::string>& file) const;

        static Option& Instance() { static Option _opt; return (_opt); }

        static bool ParseCommandLine(const char* const * argv, std::size_t argc);
        
    };  // class Option

}   // namespace sc

#define _sc_opt sc::Option::Instance()
