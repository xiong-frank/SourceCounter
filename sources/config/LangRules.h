#ifndef _Language_Rules_H_
#define _Language_Rules_H_

namespace sc
{
    using string_type = std::string;

    template<typename _Type>
    using list_type = std::vector<_Type>;

    template<typename _Key, typename _Value>
    using pair_list = list_type<std::pair<_Key, _Value>>;

    // 配置信息
    class LangRules final
    {
    public:

        struct Item
        {
            // list_type<string_type> exts;                    // 文件扩展名
            list_type<string_type> singles;                 // 单行注释符号
            pair_list<string_type, string_type> multiples;  // 多行注释符号
            pair_list<string_type, string_type> quotes;     // 字符串符号
            pair_list<string_type, string_type> primitives; // 原生字符串符号
        };

        LangRules();
        LangRules(const LangRules&) = delete;
        LangRules& operator = (const LangRules&) = delete;

    private:

        std::map<string_type, string_type> m_ExtMap;    // 文件扩展名对应的语言
        std::map<string_type, Item>  m_ItemMap;         // 语言对应的配置项

        void _AddItem(const string_type& name, const Item& item, const list_type<string_type>& exts);

    public:

        // 从文件加载配置
        bool Load(const string_type& fromFile);

        string_type Type(const string_type& ext) const;
        const Item* GetRule(const string_type& name) const;

        static LangRules& Instance() { static LangRules _lrs; return (_lrs); }

    };  // class LangRules

}

#define _sc_lrs sc::LangRules::Instance()

#endif // _Language_Rules_H_
