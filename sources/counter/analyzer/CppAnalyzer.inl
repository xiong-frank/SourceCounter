
class CppAnalyzer : public Analyzer
{
public:

    using Analyzer::Analyzer;

protected:

    virtual _symbol_t _search_begin(std::string_view& line, std::size_t& index, pair_t& arg, const item_t& item)
    {
        _symbol_t st{ _symbol_t::_nothing };

        _MatchElement<_symbol_t::_st_3>(st, line, index, std::get<2>(item), arg);
        _MatchElement<_symbol_t::_st_2>(st, line, index, std::get<1>(item), arg);
        _MatchElement<_symbol_t::_st_1>(st, line, index, std::get<0>(item), arg);

        const auto& v = std::get<3>(item).front();
        if (auto i = sc::_find_front_position(line, index, v.first); i < index)
        {
            std::string left("("), right(")");
            auto n = v.first.size();
            if (auto k = line.find(left, i + n); std::string::npos != k)
            {
                index = i;
                arg = { line.substr(i, k - i + left.size()), right.append(line.substr(i + n, k - i - n)).append(v.second) };
                st = _symbol_t::_st_4;
            }
        }

        if (_symbol_t::_st_1 < st)
            line.remove_prefix(arg.first.size() + index);

        return st;
    }

};  // class CppAnalyzer
