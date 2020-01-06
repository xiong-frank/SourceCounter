#pragma once

namespace sc
{
    // 配置规则管理器
    class RuleManager
    {
    private:

        // 文件扩展名对应的语言
        std::map<string_type, string_type, _str_compare> m_ExtMap;

        // 语言对应的语法配置项
        std::map<string_type, std::tuple<string_type, syntax_t>, _str_compare> m_SyntaxMap;

        // 默认的分析器名称
        static const string_type _DefaultName;

        // 内置支持的分析器配置
        static const std::map<string_type, std::tuple<list_t, syntax_t>> _BuildInRule;

        void _AddRule(const string_type& name, const syntax_t& item, const list_type<string_type>& exts);

    public:

        RuleManager();

        // 从文件加载配置
        bool Load(const string_type& filename, string_type& error);

        // 获取支持的语言列表
        list_t GetLanguages() const;

        // 根据文件扩展名获取对应的语言
        string_type GetLanguage(const string_type& ext) const;

        // 根据语言名称获取对应的配置规则
        const auto& GetRule(const string_type& name) const { return m_SyntaxMap.at(name); }

        // 根据语言名称获取语法配置项
        const syntax_t& GetSyntax(const string_type& name) const { return std::get<1>(GetRule(name)); }

        // 判定是否支持的语言
        bool Contains(const string_type& name) const { return (m_SyntaxMap.find(name) != m_SyntaxMap.end()); }

        // 判定是否支持的分析器
        static bool IsSupport(const string_type& name) { return (_BuildInRule.find(name) != _BuildInRule.end()); }

    };  // class RuleManager

}   // namespace sc
