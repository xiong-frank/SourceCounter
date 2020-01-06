
class RubyAnalyzer : public Analyzer
{
public:

    using Analyzer::Analyzer;

protected:

    virtual _symbol_t _search_begin(std::string_view& line, std::size_t& index, pair_t& arg, const syntax_t& item)
    {
        _symbol_t st{ _symbol_t::_nothing };

        if (const auto& symbol = std::get<1>(item).front(); 0 == line.compare(0, symbol.first.size(), symbol.first))
        {
            index = 0;
            arg = symbol;
            st = _symbol_t::_st_2;
        }
        _MatchElement<_symbol_t::_st_4>(st, line, index, std::get<3>(item), arg);
        _MatchElement<_symbol_t::_st_3>(st, line, index, std::get<2>(item), arg);
        _MatchElement<_symbol_t::_st_1>(st, line, index, std::get<0>(item), arg);

        return st;
    }

    virtual unsigned int _OnAnnotating(std::string_view& line, pair_t& arg, const syntax_t& item)
    {
        if (0 == line.compare(0, arg.second.size(), arg.second))
        {
            _status = status_t::normal;
            line.remove_prefix(arg.second.size());
            return line_t::has_comment | _OnNormal(line, arg, item);
        }

        _remove_space(line);

        return line.empty() ? line_t::is_blank : line_t::has_comment;
    }

};  // class RubyAnalyzer
