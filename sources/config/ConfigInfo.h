#ifndef _Config_Info_H_
#define _Config_Info_H_

// 配置信息
class ConfigInfo
{
private:

    unsigned int m_ThreadCount{ 0 };    // 线程数量             
    list_type m_Suffixes;               // 目标文件类型后缀列表
    list_type m_Singles;                // 单行注释符号列表
    pair_list m_Multiples;              // 多行注释标记对列表

    // 根据配置的编程语言类型调整配置
    void _AdjustConfig(const char* language);

public:

    const unsigned int& GetThreadCount() const { return m_ThreadCount; }
    const list_type& GetSuffixList() const { return m_Suffixes; }
    const list_type& GetSingleList() const { return m_Singles; }
    const pair_list& GetMultipleList() const { return m_Multiples; }

    // 显示配置
    void Show() const;

    // 从文件加载配置
    bool Load(const char* fromFile);

};

#endif // _Config_Info_H_
