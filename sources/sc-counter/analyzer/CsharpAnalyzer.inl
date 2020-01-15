
class CsharpAnalyzer : public Analyzer
{
public:

    using Analyzer::Analyzer;

protected:

    static std::size_t _find_quote(const std::string_view& view, const std::string& quote)
    {
        for (std::size_t start = 0;;)
        {
            auto index = view.find(quote, start);
            if (std::string::npos == index)
                return std::string::npos;

            if (index == view.size() - 1)
                return index;

            if (index < view.size() && '\"' == view[index + 1])
                start = index + 2;
            else
                return index;
        }
    }

    unsigned int _OnPrimitive(std::string_view& line, pair_t& arg, const syntax_t& item)
    {
        return _search_end(line_t::has_code, line, arg, item, sc::CsharpAnalyzer::_find_quote);
    }

};  // class CsharpAnalyzer
