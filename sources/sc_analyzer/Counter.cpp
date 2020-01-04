#include <future>
#include <regex>
#include <list>
#include <filesystem>

#include "../third/xf_log_console.h"
#include "../third/json.hpp"

#include "ReportType.h"
#include "Rules.h"
#include "analyzer/Analyzer.h"

#include "Counter.h"

namespace sc
{
    template<typename _Type, typename _Equal>
    bool _contains(const std::vector<_Type>& vtr, const _Type& value, _Equal equal)
    {
        for (const auto& v : vtr)
            if (equal(v, value))
                return true;

        return false;
    }

    bool _AddFile(pairs_t& files, const std::filesystem::path& file, const Rules& rules, const list_t& includes, const std::string& excludes, bool allowEmpty)
    {
        if (allowEmpty || 0 < std::filesystem::file_size(file))
        {
            if (excludes.empty() || !std::regex_search(file.generic_string(), std::regex(excludes)))
            {
                auto t = rules.GetLanguage(file.extension().generic_string());
                if (!t.empty() && (includes.empty() || _contains(includes, t, _StringEqual)))
                {
                    files.emplace_back(std::filesystem::canonical(file).generic_string(), t);
                    return true;
                }
            }
        }

        return false;
    }

    unsigned int Counter::LoadFile(const std::string& input, const std::string& excludes, list_t& includes, bool allowEmpty)
    {
        unsigned int n = 0;
        std::filesystem::path p(input);

        if (std::filesystem::is_regular_file(p))
        {
            if (_AddFile(m_Files, p, m_Rules, includes, excludes, allowEmpty))
                ++n;
        }
        else
        {
            for (const auto& iter : std::filesystem::recursive_directory_iterator(p))
                if (iter.is_regular_file() && _AddFile(m_Files, iter.path(), m_Rules, includes, excludes, allowEmpty))
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
                std::vector<std::future<std::vector<file_report_t>>> vtr;
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

    list_t Counter::Files() const
    {
        list_t result;
        for (const auto& item : m_Files)
            result.push_back(item.first);

        return result;
    }

    list_t Counter::Files(const std::string& language) const
    {
        list_t result;
        for (const auto& [filename, lang] : m_Files)
            if (lang == language)
                result.push_back(filename);

        return result;
    }

    bool Counter::_PickFile(pair_t& file)
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

    std::vector<Counter::file_report_t> Counter::_Analyze(unsigned int mode)
    {
        std::vector<file_report_t> vtr;
        for (pair_t item; _PickFile(item); )
            vtr.emplace_back(item.first, item.second, Analyzer::Analyze(item.first, item.second, *m_Rules.GetSyntax(item.second), mode));

        return vtr;
    }

}
