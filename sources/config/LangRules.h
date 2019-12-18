#ifndef _Language_Rules_H_
#define _Language_Rules_H_

namespace sc
{
    using string_type = std::string;

    template<typename _Type>
    using list_type = std::vector<_Type>;

    template<typename _Key, typename _Value>
    using pair_list = list_type<std::pair<_Key, _Value>>;

    // 自定义字符串忽略大小写比较操作
    inline int _StringIgnoreCaseCompare(const char* a, const char* b) {
        for ( ; ; ++a, ++b) {
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

    // 封装比较函数对象类型
    struct _str_compare {
        bool operator()(const string_type& a, const string_type& b) const {
            return (_StringLessThan(a, b));
        }
    };

    // 配置信息
    class LangRules final
    {
    public:

        using list_t = list_type<string_type>;
        using pairs_t = pair_list<string_type, string_type>;
        using item_t = std::tuple<list_t, pairs_t, pairs_t, pairs_t>;

        LangRules();
        LangRules(const LangRules&) = delete;
        LangRules& operator = (const LangRules&) = delete;

    private:

        std::map<string_type, string_type, _str_compare> m_ExtMap;  // 文件扩展名对应的语言
        std::map<string_type, item_t, _str_compare>  m_ItemMap;     // 语言对应的配置项

        void _AddItem(const string_type& name, const item_t& item, const list_type<string_type>& exts);

    public:

        // 从文件加载配置
        bool Load(const string_type& filename);
        bool IsSupport(const string_type& name) const;
        list_t GetLanguages() const;
        string_type GetLanguage(const string_type& ext) const;
        const item_t* GetRule(const string_type& name) const;

        static LangRules& Instance() { static LangRules _lrs; return (_lrs); }

    };  // class LangRules

}

#define _sc_lrs sc::LangRules::Instance()

#endif // _Language_Rules_H_
