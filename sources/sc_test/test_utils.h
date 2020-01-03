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

        bool _run(const _func_type& f) const
        {
            return false;
        }

    public:

        bool Add(const std::string& key, _func_type func)
        {
            auto iter = _FuncMap.emplace(key, func);
            return iter.second;
        }

        unsigned int Test() const
        {

            for (const auto& [name, func] : _FuncMap)
                _run(func);


            return 0;
        }

        unsigned int Test(const std::vector<std::string>& names) const
        {
            return 0;
        }

        void assert(bool result, const std::string& file, const std::string& func, unsigned int line)
        {

        }

        void expect(bool result, const std::string& file, const std::string& func, unsigned int line)
        {

        }

        static sc_test& Instance()
        {
            static sc_test _Inst_;
            return (_Inst_);
        }

    };
}

#define _ConcatenateName(a, b) a##b
#define _DeclareTestFunc(key) void _ConcatenateName(_test_function_name_, key)()

#define _AddTest(key, func) sc::test::sc_test::Instance().Add(key, func)

#define _SC_Test(key) _DeclareTestFunc(key); \
auto _ConcatenateName(_test_variable_name_, key) = \
_AddTest(#key, _ConcatenateName(_test_function_name_, key)); \
_DeclareTestFunc(key)

// #define expect_(expr) 
