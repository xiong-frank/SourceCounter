#include <future>
#include <regex>
#include <map>
#include <list>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "../third/xf_log_console.h"
#include "../third/json.hpp"

#include "ReportType.h"
#include "ReportItem.h"
#include "LangRules.h"
#include "analyzer/Analyzer.h"

#include "Counter.h"

namespace sc
{
    constexpr unsigned int _indent_number(10);

    unsigned int Counter::Load(const std::string& input, const std::string& config, const std::vector<std::string>& includes, const std::string& excludes, bool allowEmpty)
    {
        m_Rules.Load(config);
        auto languages = m_Rules.GetLanguages();

        if (!includes.empty())
        {
            languages = includes;
            languages.erase(std::remove_if(languages.begin(), languages.end(), [this](const auto& v) { return !m_Rules.IsSupport(v); }), languages.end());
        }

        if (languages.empty())
        {
            std::cout << "No valid language name matched." << std::endl;
            return 0;
        }

        unsigned int n = 0;
        std::filesystem::path p(input);

        if (std::filesystem::is_regular_file(p))
        {
            if (_AddFile(p, languages, excludes, allowEmpty))
                ++n;
        }
        else
        {
            for (const auto& iter : std::filesystem::recursive_directory_iterator(p))
                if (iter.is_regular_file() && _AddFile(iter.path(), languages, excludes, allowEmpty))
                    ++n;
        }

        return n;
    }

    bool Counter::Start(unsigned int nThread, unsigned int mode)
    {
        if (0 < nThread && !m_Files.empty())
        {
            if (1 < nThread)
            {
                std::vector<std::future<std::vector<FileReport>>> vtr;
                for (unsigned int i = 0; i < nThread; ++i)
                    vtr.emplace_back(std::async(std::launch::async, [this, mode]() { return this->_Analyze(mode); }));

                for (auto& f : vtr)
                {
                    auto reports = f.get();
                    std::copy(reports.begin(), reports.end(), std::back_inserter(m_Reports));
                }
            }
            else
            {
                m_Reports = _Analyze(mode);
            }

            return true;
        }

        return false;
    }

    std::vector<std::string> Counter::Files() const
    {
        std::vector<std::string> vtr;
        for (const auto& item : m_Files)
            vtr.push_back(item.first);

        return vtr;
    }

    std::vector<std::string> Counter::Files(const std::string& language) const
    {
        std::vector<std::string> vtr;
        for (const auto& [filename, lang] : m_Files)
            if (lang == language)
                vtr.push_back(filename);

        return vtr;
    }

    bool Counter::_PickFile(std::pair<std::string, std::string>& file)
    {
        /*
         * 取文件操作是互斥操作
         * 同一时刻只能有一个线程可以从文件队列取到文件
         * 如果队列为空则取文件失败，否则将取到的文件从参数带出，并将该文件从队列移除。
         */
        std::lock_guard<std::mutex> automtx(m_Mutex);

        if (m_Files.empty())
            return false;

        file = m_Files.back();
        m_Files.pop_back();

        return true;
    }

    template<typename _Type, typename _Equal>
    bool _contains(const std::vector<_Type>& vtr, const _Type& value, _Equal equal)
    {
        for (const auto& v : vtr)
            if (equal(v, value))
                return true;

        return false;
    }

    bool Counter::_AddFile(const std::filesystem::path& file, const std::vector<std::string>& langs, const std::string& excludes, bool allowEmpty)
    {
        if (allowEmpty || 0 < std::filesystem::file_size(file))
        {
            if (excludes.empty() || !std::regex_search(file.generic_string(), std::regex(excludes)))
            {
                auto t = m_Rules.GetLanguage(file.extension().generic_string());
                if (!t.empty() && (langs.empty() || _contains(langs, t, _StringEqual)))
                {
                    m_Files.emplace_back(std::filesystem::canonical(file).generic_string(), t);
                    return true;
                }
            }
        }

        return false;
    }

    std::vector<Counter::FileReport> Counter::_Analyze(unsigned int mode)
    {
        std::vector<FileReport> vtr;
        for (std::pair<std::string, std::string> item; _PickFile(item); )
            vtr.emplace_back(item.first, item.second, Analyzer::Analyze(item.first, item.second, *m_Rules.GetRule(item.second), mode));

        return vtr;
    }

}
