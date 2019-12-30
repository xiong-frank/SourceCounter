#pragma once

#include <map>
#include <functional>

namespace sc::test
{
    class sc_test final
    {
    private:

        using _func_type = std::function<void()>;
        using _func_map_t = std::map<std::string, _func_type>;

        _func_map_t _FuncMap;

        sc_test() = default;
        sc_test(const sc_test&) = delete;
        sc_test& operator = (const sc_test&) = delete;

        bool _run(const std::string& key) const;

    public:

        bool Add(const std::string& key, _func_type func)
        {
            auto iter = _FuncMap.emplace(key, func);
            return iter.second;
        }

        unsigned int Test() const;
        unsigned int Test(const std::vector<std::string>& keys) const;

        static sc_test& Instance()
        {
            static sc_test _Inst_;
            return (_Inst_);
        }

    };
}

#define _AddTest(key, func) sc::test::sc_test::Instance().Add(key, func)
