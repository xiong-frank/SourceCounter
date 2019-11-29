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

        using list_t = list_type<string_type>;
        using pairs_t = pair_list<string_type, string_type>;
        using item_t = std::tuple<list_t, pairs_t, pairs_t, pairs_t>;

        LangRules();
        LangRules(const LangRules&) = delete;
        LangRules& operator = (const LangRules&) = delete;

    private:

        std::map<string_type, string_type> m_ExtMap;    // 文件扩展名对应的语言
        std::map<string_type, item_t>  m_ItemMap;         // 语言对应的配置项

        void _AddItem(const string_type& name, const item_t& item, const list_type<string_type>& exts);

    public:

        // 从文件加载配置
        bool Load(const string_type& filename);

        list_type<string_type> GetLanguage() const;
        string_type GetLanguage(const string_type& ext) const;
        const item_t* GetRule(const string_type& name) const;

        static LangRules& Instance() { static LangRules _lrs; return (_lrs); }

    };  // class LangRules

}

#define _sc_lrs sc::LangRules::Instance()

#endif // _Language_Rules_H_
