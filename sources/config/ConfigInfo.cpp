#include <string>
#include <queue>
#include <list>
#include <fstream>
#include <iostream>

#include "utils/PathUtils.h"
#include "log/sc_log.h"
#include "config/ConfigInfo.h"

// Example:
//------------------------------------------------------------------------------
// 
// # C++
// Suffixes     = .cpp .h .c .inl .hpp .cxx .cc .rc .def
// Singles      = //
// Multiples    = /*:*/
// 
// # Java
// Suffixes     = .java
// Singles      = //
// Multiples    = /*:*/
// 
// # Python
// Suffixes     = .py .pyw
// Singles      = "#"
// Multiples    = ''':''' """:"""
// 
// # Clojure
// Suffixes     = .clj
// Singles      = ; ;; ;;; ;;;;
// Multiples    = (comment":")
//
//------------------------------------------------------------------------------

bool AnalyzeConfigLine(std::string& key, list_type& value, char* buf);
bool AnalyzeValueList(list_type& value, char* ptr);

void ConfigInfo::Show() const
{
#ifdef __SC_LOG_

    _log("ThreadCount: %d", m_ThreadCount);

    _log("Suffixes:%s", sc_log::_MakeText(m_Suffixes).c_str());

    _log("Singles:%s", sc_log::_MakeText(m_Singles).c_str());

    char buf[sc_log::_MaxLog]{ 0 };
    unsigned int index = 0;
    for (auto& pair : m_Multiples)
    {
        int n = sprintf_s(buf + index, sc_log::_MaxLog - index, " {%s:%s}", pair.first.c_str(), pair.second.c_str());
        if (n < 0)
            break;

        index += n;
        if (sc_log::_MaxLog <= index)
            break;
    }
    _log("Multiples:%s", buf);

#endif
}

bool ConfigInfo::Load(const char * configFile)
{
    std::ifstream fin;
    fin.open(configFile);
    if (fin.is_open())
    {
        std::string language;   // 记录是否有编程语言配置

        for (;;)
        {
            char buf[0x0400]{ 0 };
            if (!fin.getline(buf, 0x0400))
                break;
            
            std::string key;
            list_type value;

            if (AnalyzeConfigLine(key, value, buf))
            {
                if (0 == strcmp("threadcount", key.c_str()))
                {
                    try
                    {
                        // 解析线程数量
                        if (!value.empty())
                            m_ThreadCount = stoul(value.front(), nullptr, 10);
                    }
                    catch (const std::exception& _ex)
                    {
                        _log("analyze thread count config occur exception: %s", _ex.what());
                    }
                }
                else if (0 == strcmp("suffixes", key.c_str()))
                {
                    // 加载后缀名列表
                    for (std::string& v : value)
                    {
                        // 全部采用小写
                        std::for_each(v.begin(), v.end(), [](char& c) { c = tolower(c); });
                        // 防止重复添加
                        if (m_Suffixes.end() == std::find(m_Suffixes.begin(), m_Suffixes.end(), v))
                            m_Suffixes.emplace_back(v);
                    }
                }
                else if (0 == strcmp("singles", key.c_str()))
                {
                    // 加载单行注释符号列表
                    for (std::string& v : value)
                    {
                        if (m_Singles.end() == std::find(m_Singles.begin(), m_Singles.end(), v))
                            m_Singles.emplace_back(v);
                    }
                }
                else if (0 == strcmp("multiples", key.c_str()))
                {
                    // 加载多行注释符号列表
                    for (std::string& p : value)
                    {
                        // 按冒号拆分多行注释起始和结束标记对
                        std::size_t pos = p.find(':');
                        if (0 < pos && pos < p.size() - 1)
                        {
                            std::pair<std::string, std::string> v{ std::string(p, 0, pos), std::string(p, pos + 1, p.size() - pos - 1) };
                            if (m_Multiples.end() == std::find(m_Multiples.begin(), m_Multiples.end(), v))
                                m_Multiples.emplace_back(v);
                        }
                    }
                }
                else
                {
                    if (language.empty() && 0 == strcmp("language", key.c_str()) && !value.empty())
                        language = value.front();
                }
            }
        }

        fin.close();

        if (m_ThreadCount == 0)
            m_ThreadCount = 2;

        if (!language.empty())
            _AdjustConfig(language.c_str());

        return true;
    }
    else
    {
        // 如果没有读取到配置文件，默认按 C++ 语法配置
        m_ThreadCount = 4;
        m_Suffixes = { ".cpp", ".h", ".c", ".inl", ".hpp", ".cxx", ".cc", ".rc", ".def" };
        m_Singles = { "//" };
        m_Multiples = { { "/*", "*/" } };

        _log("can\'t load file: %s, use the C++ config by default !", configFile);
    }

    return false;
}

void ConfigInfo::_AdjustConfig(const char * language)
{
    /*
     * 如果配置文件中指定了编程语言名称
     * 将根据对应的语言默认填充缺失的配置信息
     */
    if (0 == strcmp("c++", language))
    {
        if (m_Suffixes.empty())
            m_Suffixes = { ".cpp", ".h", ".c", ".inl", ".hpp", ".cxx", ".cc", ".rc", ".def" };
        if (m_Singles.empty())
            m_Singles = { "//" };
        if (m_Multiples.empty())
            m_Multiples = { { "/*", "*/" } };
    }
    else if (0 == strcmp("java", language))
    {
        if (m_Suffixes.empty())
            m_Suffixes = { ".java" };
        if (m_Singles.empty())
            m_Singles = { "//" };
        if (m_Multiples.empty())
            m_Multiples = { { "/*", "*/" } };
    }
    else if (0 == strcmp("python", language))
    {
        if (m_Suffixes.empty())
            m_Suffixes = { ".py", ".pyw" };
        if (m_Singles.empty())
            m_Singles = { "#" };
        if (m_Multiples.empty())
            m_Multiples = { { R"(''')", R"(''')" },{ R"(""")", R"(""")" } };
    }
    else if (0 == strcmp("clojure", language))
    {
        if (m_Suffixes.empty())
            m_Suffixes = { ".clj" };
        if (m_Singles.empty())
            m_Singles = { ";", ";;", ";;;", ";;;;" };
        if (m_Multiples.empty())
            m_Multiples = { { "(comment\"", "\")" } };
    }
    else
    {
        _log("the language: %s don\'t have default config", language);
    }
}

// 解析配置行
bool AnalyzeConfigLine(std::string& key, list_type& value, char* line)
{
    char* ptr = strchr(line, '=');
    if (nullptr == ptr || ptr == line)
        return false;

    char *k1 = line, *k2 = ptr - 1;

    for (; k1 != k2; ++k1)
        if (*k1 != ' ')
            break;

    for (; k1 != k2; --k2)
        if (*k2 != ' ')
            break;

    if (k2 - k1 < 1)
        return false;

    if (*k1 == '\"' && *k2 == '\"')
    {
        ++k1;
    }
    else
    {
        if (*k1 != '\"' && *k2 != '\"')
            ++k2;
        else
            return false;
    }

    bool quoting = false;
    char *start = ++ptr;
    for (; ; ++ptr)
    {
        if (quoting)
        {
            for (; ; ++ptr)
            {
                if (*ptr == 0)
                    return false;

                if (*ptr == '\"')
                {
                    if (*(ptr - 1) != '\\')
                    {
                        *ptr = ' ';
                        quoting = false;
                        break;
                    }
                }
            }
        }
        else
        {
            for (; ; ++ptr)
            {
                if (*ptr == 0 || *ptr == '#')
                {
                    *ptr = 0;
                    if (AnalyzeValueList(value, start))
                    {
                        // 将 key 转换成小写
                        std::for_each(k1, k2, [](char& c) { c = tolower(c); });
                        key = { k1, unsigned int(k2 - k1) };
                        return true;
                    }

                    return false;
                }

                if (*ptr == '\"')
                {
                    if (*(ptr - 1) != '\\')
                    {
                        *ptr = ' ';
                        quoting = true;
                        break;
                    }
                }
            }
        }
    }
}

// 按空格分隔拆分值列表
bool AnalyzeValueList(list_type& value, char* ptr)
{
    for (; *ptr; ++ptr)
    {
        if (*ptr != ' ')
        {
            char* start = ptr;
            for (++ptr; *ptr; ++ptr)
                if (*ptr == ' ')
                    break;

            std::string v(start, ptr - start);
            // 移除 \" 前面的 '\'
            for (;;)
            {
                std::size_t pos = v.find("\\\"");
                if (std::string::npos == pos)
                    break;
                else
                    v.erase(pos, 1);
            }

            value.emplace_back(v);
        }
    }

    return !value.empty();
}
