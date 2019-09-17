#ifndef _Language_Config_H_
#define _Language_Config_H_

// 配置信息
class Config
{
public:

    using string_type = std::string;

    template<typename _Type>
    using list_type = std::vector<_Type>;

    template<typename _Key, typename _Value>
    using pair_list = list_type<std::pair<_Key, _Value>>;

    struct Item
    {
        list_type<string_type> singles;
        pair_list<string_type, string_type> multiples;
        pair_list<string_type, string_type> quotes;
    };

private:

    std::map<string_type, Item>  m_ItemMap;         // 语言对应的配置项
    std::map<string_type, string_type> m_ExtMap;    // 文件扩展名对应的语言

public:

    // 从文件加载配置
    bool Load(const string_type& fromFile);

    bool FindConfig(const string_type& suffix) const;

};

#endif // _Language_Config_H_
