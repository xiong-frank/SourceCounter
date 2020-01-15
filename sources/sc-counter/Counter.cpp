#include <future>
#include <regex>
#include <list>
#include <filesystem>

#include "../third/xf_log_console.h"

#include "Counter.h"
#include "analyzer/Analyzer.h"

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

    bool _AddItem(std::vector<Counter::item_t>& items, const RuleManager& mgr, const std::filesystem::path& file, const list_t& includes, const std::string& excludes, bool allowEmpty)
    {
        if (allowEmpty || 0 < std::filesystem::file_size(file))
        {
            if (excludes.empty() || !std::regex_search(file.generic_string(), std::regex(excludes)))
            {
                const auto& name = mgr.GetLanguage(file.extension().generic_string());
                if (!name.empty() && (includes.empty() || _contains(includes, name, _StringEqual)))
                {
                    const auto& [type, syntax] = mgr.GetRule(name);
                    items.emplace_back(std::filesystem::canonical(file).generic_string(), name, type, syntax);

                    return true;
                }
            }
        }

        return false;
    }

    unsigned int Counter::LoadFile(const std::string& input, const std::string& excludes, const list_t& includes, bool allowEmpty)
    {
        unsigned int n = 0;
        
        if (std::filesystem::path p(input); std::filesystem::exists(p))
        {
            if (std::filesystem::is_regular_file(p))
            {
                if (_AddItem(m_Items, m_RuleMgr, p, includes, excludes, allowEmpty))
                    ++n;
            }
            else
            {
                for (const auto& iter : std::filesystem::recursive_directory_iterator(p))
                    if (iter.is_regular_file() && _AddItem(m_Items, m_RuleMgr, iter.path(), includes, excludes, allowEmpty))
                        ++n;
            }
        }
        else
        {
            _xfLog(R"("%s" is not a valid path, load sources failed.)", input.c_str());
        }

        return n;
    }

    bool Counter::Start(unsigned int nThread, unsigned int mode)
    {
        if (0 < nThread && !m_Items.empty())
        {
            _xfLog("Start counting source lines, thread: %d, mode: %d, files: %zd", nThread, mode, m_Items.size());

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

        _xfLog("There are no items to analyze, or the thread could not be started.");
        return false;
    }

    std::vector<Counter::file_report_t> Counter::_Analyze(unsigned int mode)
    {
        std::vector<file_report_t> vtr;
        for (unsigned int i = m_ItemIndex++; i < m_Items.size(); i = m_ItemIndex++)
        {
            const auto& [file, name, type, syntax] = m_Items[i];
            vtr.emplace_back(file, name, sc::Analyzer::Analyze(file, type, syntax, mode));
        }

        return vtr;
    }

    list_t Counter::Files() const
    {
        list_t result;
        for (const auto& item : m_Items)
            result.push_back(std::get<0>(item));

        return result;
    }

    list_t Counter::Files(const std::string& language) const
    {
        list_t result;
        for (const auto& [file, name, _ignore1, _ignore2] : m_Items)
            if (name == language)
                result.push_back(file);

        return result;
    }

}
