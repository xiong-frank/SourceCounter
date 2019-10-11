/*
command line parameter parsing library for Modern C++
version 1.0.0
https://github.com/xf-bnb/CommandLineParser

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX - License - Identifier : MIT
Copyright(c) 2019 Frank Xiong <https://github.com/xf-bnb>.
*/

#pragma once

#include <functional>
#include <regex>
#include <variant>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace xf::cmd
{
    inline const char* version() { return "1.0.0"; }

    using string_t = std::string;
    using size_type = std::size_t;

    template<typename _Type>
    using set_t = std::set<_Type>;

    template<typename _Type>
    using list_t = std::vector<_Type>;

    template<typename _KeyType, typename _ValueType>
    using pair_t = std::pair<_KeyType, _ValueType>;

    template<typename _KeyType, typename _ValueType>
    using map_t = std::map<_KeyType, _ValueType>;

    enum class value_t : unsigned char {
        vt_string, vt_integer, vt_unsigned, vt_float, vt_boolean, vt_nothing
    };

    class option_t
    {
    public:

        static inline bool is_string(const string_t& v) { return !v.empty(); }
        static inline bool is_boolean(const string_t& v) { return std::regex_match(v, std::regex("[Tt]rue|[Ff]alse|TRUE|FALSE")); }
        static inline bool is_integer(const string_t& v) { return std::regex_match(v, std::regex("[+-]?(0|[1-9][0-9]*)")); }
        static inline bool is_unsigned(const string_t& v) { return std::regex_match(v, std::regex("0|[1-9][0-9]*")); }
        static inline bool is_float(const string_t& v) { return std::regex_match(v, std::regex("[+-]?(0|[1-9][1-9]*)([.][0-9]+)?")); }

    private:

        using _CallType = std::function<bool (const string_t&)>;

        value_t _vt;
        bool _is_unique;
        bool _k_required;
        bool _v_required;
        _CallType _check;

        template<typename _ValueType> struct _type_mapper;
        template<> struct _type_mapper<bool> { static constexpr value_t _value = value_t::vt_boolean; };
        template<> struct _type_mapper<int> { static constexpr value_t _value = value_t::vt_integer; };
        template<> struct _type_mapper<unsigned int> { static constexpr value_t _value = value_t::vt_unsigned; };
        template<> struct _type_mapper<double> { static constexpr value_t _value = value_t::vt_float; };
        template<> struct _type_mapper<string_t> { static constexpr value_t _value = value_t::vt_string; };
        template<> struct _type_mapper<nullptr_t> { static constexpr value_t _value = value_t::vt_nothing; };

        static constexpr bool (*_func_list[])(const string_t&){ is_string, is_integer, is_unsigned, is_float, is_boolean, nullptr };

    public:

        option_t() = default;

        option_t(const value_t& vt, bool u, bool k, bool v, _CallType _func)
            : _vt(vt), _is_unique(u), _k_required(k), _v_required(v), _check(_func)
        { }

        option_t(const value_t& vt, bool u, bool k, bool v, const string_t& expr)
            : option_t(vt, u, k, v, [expr](const string_t& value) { return std::regex_match(value, std::regex(expr)); })
        { }

        option_t(const value_t& vt, bool u, bool k, bool v)
            : option_t(vt, u, k, v, _func_list[static_cast<size_type>(vt)])
        { }

        option_t(bool u, bool k)
            : option_t(value_t::vt_nothing, u, k, false, _CallType())
        { }

        const value_t& value_type() const { return _vt; }
        bool is_unique() const { return _is_unique; }
        bool is_key_required() const { return _k_required; }
        bool is_value_required() const { return _v_required; }
        bool check(const string_t& value) const { return (_check && (_check(value))); }

        template<typename _ValueType, typename _CheckType>
        static option_t make(bool u, bool k, bool v, _CheckType checker) {
            return option_t(_type_mapper<_ValueType>::_value, u, k, v, checker);
        }

        template<typename _ValueType>
        static option_t make(bool u, bool k, bool v) {
            return option_t(_type_mapper<_ValueType>::_value, u, k, v);
        }

        template<typename _ValueType> static option_t make(bool, bool);
        template<> static option_t make<nullptr_t>(bool u, bool k) { return option_t(u, k); }

    };

    class Parser
    {
    public:

        enum state_t {
            s_ok,                   // ok
            s_nothing,              // 没有任何参数
            s_k_unrecognized,       // 参数不能识别
            s_k_duplicated,         // 参数重复
            s_k_conflict,           // 参数唯一性冲突
            s_k_missing,            // 缺少必需的参数
            s_v_missing,            // 参数值缺失
            s_v_redundant,          // 参数值多余
            s_v_error               // 参数值错误
        };

        class result_t
        {
            friend class Parser;

            using variant_t = std::variant<nullptr_t, bool, int, unsigned int, double, string_t>;

            template<typename _Type> static string_t _to_string(const _Type& v) { return std::to_string(v); }
            template<> static string_t _to_string(const nullptr_t& v) { return ""; }
            template<> static string_t _to_string(const bool& v) { return (v ? "true" : "false"); }
            template<> static string_t _to_string(const string_t& v) { return v; }
            template<> static string_t _to_string(const variant_t& v)
            {
                string_t x;
                std::visit([&](auto&& t) mutable { x = result_t::_to_string(t); }, v);
                return x;
            }

            state_t _state;
            string_t _info;
            pair_t<string_t, string_t> _extra;
            map_t<string_t, string_t> _k_map;
            map_t<string_t, variant_t> _v_map;
            bool _is_unique{ false };

            result_t(state_t code, const string_t& text) : _state(code), _info(text) { }

        public:

            state_t code() const { return _state; }
            const string_t& info() const { return _info; }
            const pair_t<string_t, string_t>& hint() const { return _extra; }
            bool is_valid() const { return (state_t::s_ok == code()); }
            bool is_existing(const string_t& key) const { return _k_map.find(key) != _k_map.end(); }

            operator bool() const { return is_valid(); }
            operator const string_t& () const { return info(); }

            bool has_value(const string_t& key) const
            {
                auto k_iter = _k_map.find(key);
                if (k_iter == _k_map.end())
                    return false;

                auto v_iter = _v_map.find(k_iter->second);
                if (v_iter == _v_map.end())
                    return false;

                return (static_cast<size_type>(value_t::vt_nothing) < v_iter->second.index());
            }

            template<typename _Type>
            _Type get(const string_t& key) const
            {
                return std::get<_Type>(_v_map.at(_k_map.at(key)));
            }

            template<typename _Type>
            _Type get(const string_t& key, const _Type& value) const
            {
                try {
                    return get<_Type>(key);
                } catch (const std::exception& /* e */) {
                    // std::cout << e.what() << std::endl;
                    return value;
                }
            }

            const map_t<string_t, variant_t>& get() const { return _v_map; }
            
            map_t<string_t, string_t> args() const
            {
                map_t<string_t, string_t> mss;
                for (auto v : _v_map)
                    mss.emplace(v.first, _to_string(v.second));

                return mss;
            }

        private:

            static string_t _make_info(state_t s, const string_t& a, const string_t& b)
            {
                switch (s)
                {
                case state_t::s_ok:
                    return "ok";
                case state_t::s_nothing:
                    return R"(error: don't get any parameter.)";
                case state_t::s_k_unrecognized:
                    return R"(error: unrecognized parameter ")" + a + R"(".)";
                case state_t::s_k_duplicated:
                    return R"(error: repeat paramter ")" + a + R"(" and ")" + b + R"(".)";
                case state_t::s_k_missing:
                    return R"(error: the parameter ")" + a + R"(" must be specified but not found.)";
                case state_t::s_v_missing:
                    return R"(error: parameter ")" + a + R"(" must specify a value.)";
                case state_t::s_v_redundant:
                    return R"(error: the parameter ")" + a + R"(" doesn't require value.)";
                case state_t::s_v_error:
                    return R"(error: the ")" + b + R"(" can't be treated as the value of parameter ")" + a + R"(".)";
                case state_t::s_k_conflict:
                    return R"(error: parameter ")" + a + R"(" can't be specified with other parameters.)";
                default:
                    return string_t();
                }
            }

            void _set_error(state_t s, const string_t& a = string_t(), const string_t& b = string_t())
            {
                _state = s;
                _extra.first = a;
                _extra.second = b;

                _info = _make_info(_state, _extra.first, _extra.second);
            }

            bool _check_key(const string_t& key, const option_t& opt)
            {
                if (_k_map.empty())
                {
                    if (opt.is_unique()) _is_unique = true;
                    return true;
                }

                if (_is_unique)
                {
                    _set_error(state_t::s_k_conflict, _v_map.begin()->first);
                    return false;
                }

                if (opt.is_unique())
                {
                    _set_error(state_t::s_k_conflict, key);
                    return false;
                }

                auto iter = _k_map.find(key);
                if (iter != _k_map.end())
                {
                    _set_error(state_t::s_k_duplicated, iter->second, key);
                    return false;
                }

                return true;
            }

            template<typename _Type>
            void _add_value(const string_t& key, const _Type& value, const set_t<string_t>& keys)
            {
                _v_map.emplace(key, variant_t(value));
                for (auto k : keys) _k_map.emplace(k, key);
            }

            void _add_value(const string_t& key, const string_t& value, const set_t<string_t>& keys, const option_t& opt)
            {
                switch (opt.value_type())
                {
                case value_t::vt_string:
                    _add_value(key, value, keys);
                    break;
                case value_t::vt_boolean:
                    _add_value(key, ('t' == value[0] || 'T' == value[0]), keys);
                    break;
                case value_t::vt_float:
                    _add_value(key, std::stod(value), keys);
                    break;
                case value_t::vt_integer:
                    _add_value(key, std::stoi(value), keys);
                    break;
                case value_t::vt_unsigned:
                    _add_value(key, unsigned int(std::stoul(value)), keys);
                    break;
                case value_t::vt_nothing:
                    _add_value(key, nullptr, keys);
                    break;
                default:
                    break;
                }
            }
            
            void _add_value(const string_t& key, const set_t<string_t>& keys)
            {
                _add_value(key, nullptr, keys);
            }
            
        };

    public:

        Parser() = default;

        Parser(const list_t<pair_t<set_t<string_t>, option_t>>& options)
        {
            for (auto opt : options) AddOption(opt);
        }

        Parser& AddOption(const pair_t<set_t<string_t>, option_t>& option)
        {
            if (!option.first.empty())
            {
                _update_option_id();

                opt_map.emplace(option_id, option);

                for (auto key : option.first)
                {
                    auto iter = key_map.find(key);
                    if (iter != key_map.end())
                    {
                        _RemoveOption(iter->second, key);
                        iter->second = option_id;
                    }
                    else
                    {
                        key_map.emplace(key, option_id);
                    }
                }
            }

            return *this;
        }

        size_type RemoveOption(const set_t<string_t>& keys)
        {
            size_type n = 0;
            for (auto key : keys)
            {
                auto iter = key_map.find(key);
                if (iter != key_map.end())
                {
                    _RemoveOption(iter->second, key);
                    key_map.erase(iter);
                    ++n;
                }
            }

            return n;
        }

        bool IsValid(const string_t& key) const
        {
            return (key_map.find(key) != key_map.end());
        }

        bool IsSame(const string_t& k1, const string_t& k2) const
        {
            auto iter1 = key_map.find(k1);
            if (iter1 == key_map.end()) return false;

            auto iter2 = key_map.find(k2);
            if (iter2 == key_map.end()) return false;

            return (iter1->second == iter2->second);
        }

        const option_t* GetOption(const string_t& key) const
        {
            auto iter = key_map.find(key);
            if (iter != key_map.end())
                return &opt_map.at(iter->second).second;

            return nullptr;
        }

        set_t<string_t> GetKeys(const string_t& key) const
        {
            set_t<string_t> keys;
            auto iter = key_map.find(key);
            if (iter != key_map.end())
                keys = opt_map.at(iter->second).first;

            return keys;
        }

        set_t<string_t> GetKeys() const
        {
            set_t<string_t> keys;
            for (auto v : key_map)
                keys.emplace(v.first);

            return keys;
        }

        using const_char_ptr = const string_t::value_type*;

        result_t Parse(const const_char_ptr* argv, size_type from, unsigned int to) const
        {
            return Parse(list_t<string_t>(argv + from, argv + to));
        }

        template<size_type n>
        result_t Parse(const const_char_ptr (&argv)[n], unsigned int from) const
        {
            return Parse(argv, from, n);
        }
        
        template<size_type n>
        result_t Parse(const const_char_ptr (&argv)[n]) const
        {
            return Parse(argv, 0, n);
        }

        result_t Parse(const list_t<string_t>& args) const
        {
            if (args.empty())
                return result_t(state_t::s_nothing, R"(error: don't get any parameter.)");

            return _Parse(args, { &Parser::_OnKey, &Parser::_OnValue, &Parser::_OnOptional });
        }

    private:

        using id_type = unsigned int;
        using _parse_func_type = size_type(Parser::*)(result_t&, const string_t&, const list_t<string_t>&, string_t&, option_t&) const;

        id_type option_id{ 0 };
        map_t<string_t, id_type> key_map;
        map_t<id_type, pair_t<set_t<string_t>, option_t>> opt_map;

        enum { on_key, on_value, on_opt, parse_error };

        void _update_option_id() { ++option_id; }

        const option_t& _Option(const string_t& key) const
        {
            return opt_map.at(key_map.at(key)).second;
        }

        const set_t<string_t>& _Keys(const string_t& key) const
        {
            return opt_map.at(key_map.at(key)).first;
        }

        template<size_type n>
        result_t _Parse(const list_t<string_t>& args, const _parse_func_type(&_parse_functions)[n]) const
        {
            string_t key;
            option_t opt;
            size_type index(on_key);

            list_t<string_t> keys;
            for (auto v : key_map) keys.emplace_back(v.first);
            std::sort(keys.begin(), keys.end(), [](const string_t& a, const string_t& b) { return (b.size() < a.size()); });

            result_t result(state_t::s_ok, "ok");

            for (auto arg : args)
            {
                if (index < n)
                    index = (this->*_parse_functions[index])(result, arg, keys, key, opt);
                else
                    break;
            }

            switch (index)
            {
            case on_opt:
                result._add_value(key, _Keys(key));
            case on_key:
                _CheckResult(result);
                break;
            case on_value:
                result._set_error(state_t::s_v_missing, key);
                break;
            default:
                break;
            }

            return result;
        }

        size_type _OnKey(result_t& result, const string_t& arg, const list_t<string_t>& keys, string_t& k, option_t& opt) const
        {
            for (auto key : keys)
            {
                if (_start_with(arg, key))
                {
                    if (_is_perfect_match(arg, key))
                        return _OnPerfectMatch(result, key, k, opt);

                    if (_is_equation(arg, key))
                        return _OnEquation(result, key, arg.substr(key.size() + 1), _Option(key));

                    break;
                }
            }

            result._set_error(state_t::s_k_unrecognized, arg);
            return parse_error;
        }

        size_type _OnValue(result_t& result, const string_t& arg, const list_t<string_t>& keys, string_t& k, option_t& opt) const
        {
            return _OnValueEx(result, k, arg, opt);
        }

        size_type _OnOptional(result_t& result, const string_t& arg, const list_t<string_t>& keys, string_t& k, option_t& opt) const
        {
            if (opt.is_unique())
                return _OnValueEx(result, k, arg, opt);

            for (auto key : keys)
            {
                if (_start_with(arg, key))
                {
                    if (_is_perfect_match(arg, key))
                    {
                        result._add_value(k, _Keys(k));
                        return _OnPerfectMatch(result, key, k, opt);
                    }

                    if (_is_equation(arg, key))
                    {
                        result._add_value(k, _Keys(k));
                        return _OnEquation(result, key, arg.substr(key.size() + 1), _Option(key));
                    }

                    break;
                }
            }

            return _OnValue(result, arg, keys, k, opt);
        }

        size_type _OnPerfectMatch(result_t& result, const string_t& key, string_t& k, option_t& opt) const
        {
            opt = _Option(key);
            if (!result._check_key(key, opt))
                return parse_error;

            k = key;
            if (value_t::vt_nothing == opt.value_type())
            {
                result._add_value(key, nullptr, _Keys(key));
                return on_key;
            }

            return (opt.is_value_required() ? on_value : on_opt);
        }

        size_type _OnEquation(result_t& result, const string_t& key, const string_t& value, const option_t& opt) const
        {
            if (!result._check_key(key, opt))
                return parse_error;

            if (value_t::vt_nothing == opt.value_type())
            {
                result._set_error(state_t::s_v_redundant, key);
                return parse_error;
            }

            return _OnValueEx(result, key, value, opt);
        }

        size_type _OnValueEx(result_t& result, const string_t& key, const string_t& value, const option_t& opt) const
        {
            if (opt.check(value))
            {
                result._add_value(key, value, _Keys(key), opt);
                return on_key;
            }

            result._set_error(state_t::s_v_error, key, value);
            return parse_error;
        }

        bool _RemoveOption(id_type id, const string_t& key)
        {
            auto iter = opt_map.find(id);
            if (iter != opt_map.end())
            {
                iter->second.first.erase(key);
                if (iter->second.first.empty())
                    opt_map.erase(iter);

                return true;
            }

            return false;
        }

        bool _CheckResult(result_t& result) const
        {
            if (state_t::s_ok == result.code())
            {
                if (_Option(result._k_map.cbegin()->first).is_unique())
                    return true;

                for (auto opt : opt_map)
                {
                    if (opt.second.second.is_key_required())
                    {
                        const string_t& key(*opt.second.first.cbegin());
                        if (!result.is_existing(key))
                        {
                            result._set_error(state_t::s_k_missing, key);
                            return false;
                        }
                    }
                }

                return true;
            }

            return false;
        }

        static bool _is_perfect_match(const string_t& text, const string_t& key)
        {
            return (text.size() == key.size());
        }

        static bool _is_equation(const string_t& text, const string_t& key)
        {
            return ((key.size() + 1) < text.size() && '=' == text[key.size()]);
        }

        static bool _start_with(const string_t& a, const string_t& b)
        {
            return 0 == a.compare(0, b.size(), b);
        }

    };

}
