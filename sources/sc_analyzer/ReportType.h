#pragma once

namespace sc
{

    using string_type = std::string;

    template<typename _Type>
    using list_type = std::vector<_Type>;

    template<typename _Key, typename _Value>
    using pair_list = list_type<std::pair<_Key, _Value>>;

    using list_t = list_type<string_type>;
    using pair_t = std::pair<string_type, string_type>;
    using pairs_t = pair_list<string_type, string_type>;
    using item_t = std::tuple<list_t, pairs_t, pairs_t, pairs_t>;

    // 自定义字符串忽略大小写比较函数
    inline int _StringIgnoreCaseCompare(const char* a, const char* b) {
        for (; ; ++a, ++b) {
            unsigned char x = *a;
            unsigned char y = *b;
            if ('A' <= x && x <= 'Z') x |= 0x20;
            if ('A' <= y && y <= 'Z') y |= 0x20;
            if (0 == x || x != y) return (x - y);
        }
    }

    inline bool _StringLessThan(const string_type& a, const string_type& b) {
        return (_StringIgnoreCaseCompare(a.c_str(), b.c_str()) < 0);
    }

    inline bool _StringEqual(const string_type& a, const string_type& b) {
        return (a.size() == b.size()) && (_StringIgnoreCaseCompare(a.c_str(), b.c_str()) == 0);
    }

    // 忽略大小写的字符串比较函数类型
    struct _str_compare {
        bool operator()(const string_type& a, const string_type& b) const {
            return (_StringLessThan(a, b));
        }
    };

    // 定义行解释模式
    enum mode_t
    {
        cc_is_code      = 0x01,
        cc_is_comment   = 0x02,
        mc_is_blank     = 0x04,
        mc_is_comment   = 0x08,
        ms_is_blank     = 0x10,
        ms_is_code      = 0x20
    };

    inline bool _check_mode(unsigned int m, unsigned int k) { return ((m & k) == k); }

}