#pragma once

namespace sc
{
    // 配置信息
    class Rules
    {
    private:

        std::map<string_type, string_type, _str_compare> m_ExtMap;      // 文件扩展名对应的语言
        std::map<string_type, syntax_t, _str_compare>  m_SyntaxMap;     // 语言对应的语法配置项

        void _AddRule(const string_type& name, const syntax_t& item, const list_type<string_type>& exts);

    public:

        Rules();

        bool Load(const string_type& filename);
        bool IsSupport(const string_type& name) const;
        list_t GetLanguages() const;
        string_type GetLanguage(const string_type& ext) const;
        const syntax_t* GetSyntax(const string_type& name) const;

    };  // class Rules

}
