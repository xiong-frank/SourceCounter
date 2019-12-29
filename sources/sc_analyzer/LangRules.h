#pragma once

namespace sc
{
    // 配置信息
    class LangRules final
    {
    public:

        LangRules();

    private:

        std::map<string_type, string_type, _str_compare> m_ExtMap;  // 文件扩展名对应的语言
        std::map<string_type, item_t, _str_compare>  m_ItemMap;     // 语言对应的配置项

        void _AddItem(const string_type& name, const item_t& item, const list_type<string_type>& exts);

    public:

        bool Load(const string_type& filename);
        bool IsSupport(const string_type& name) const;
        list_t GetLanguages() const;
        string_type GetLanguage(const string_type& ext) const;
        const item_t* GetRule(const string_type& name) const;

    };  // class LangRules

}
