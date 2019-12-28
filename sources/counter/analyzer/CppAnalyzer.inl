
class CppAnalyzer : public Analyzer
{
    static constexpr std::size_t _delimiter_length{ 0x10 };

public:

    using Analyzer::Analyzer;

protected:

    virtual _symbol_t _search_begin(std::string_view& line, std::size_t& index, pair_t& arg, const item_t& item)
    {
        _symbol_t st{ _symbol_t::_nothing };

        _MatchElement<_symbol_t::_st_3>(st, line, index, std::get<2>(item), arg);
        _MatchElement<_symbol_t::_st_2>(st, line, index, std::get<1>(item), arg);
        _MatchElement<_symbol_t::_st_1>(st, line, index, std::get<0>(item), arg);

        const auto& v = std::get<3>(item)[0];
        const auto& x = std::get<3>(item)[1];
        if (auto i = sc::_find_front_position(line, index, v.first); i < index)
        {
            auto n = v.first.size();
            if (auto k = line.find(x.first, i + n); (std::string::npos != k && (k - i - n) <= _delimiter_length))
            {
                index = i;
                arg.first = line.substr(i, k - i + x.first.size());
                arg.second = std::string(x.second).append(line.substr(i + n, k - i - n)).append(v.second);
                st = _symbol_t::_st_4;
            }
        }

        if (_symbol_t::_st_1 < st)
            line.remove_prefix(arg.first.size() + index);

        return st;
    }

};  // class CppAnalyzer
