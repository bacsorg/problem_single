#include "bacs/single/problem/detail/resource.hpp"

#include <string>
#include <algorithm>

#include <cmath>

#include <boost/cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace bacs{namespace single{namespace problem{namespace detail
{
    namespace qi = boost::spirit::qi;

    struct si_multiple: qi::symbols<char, double>
    {
        si_multiple()
        {
            add
                ("da", 1e1)
                ("h", 1e2)
                ("k", 1e3)
                ("M", 1e6)
                ("G", 1e9)
                ("T", 1e12)
                ("P", 1e15)
                ("E", 1e18)
                ("Z", 1e21)
                ("Y", 1e24);
        }
    };

    struct si_submultiple: qi::symbols<char, double>
    {
        si_submultiple()
        {
            add
                ("d", 1e-1)
                ("c", 1e-2)
                ("m", 1e-3)
                ("u", 1e-6)
                ("n", 1e-9)
                ("p", 1e-12)
                ("f", 1e-15)
                ("a", 1e-18)
                ("z", 1e-21)
                ("y", 1e-24);
        }
    };

    struct binary_multiple: qi::symbols<char, double>
    {
        binary_multiple()
        {
            constexpr double ten = 1 << 10;
            double mult = 1;
            add("Ki", mult *= ten);
            add("Mi", mult *= ten);
            add("Gi", mult *= ten);
            add("Ti", mult *= ten);
            add("Pi", mult *= ten);
            add("Ei", mult *= ten);
            add("Zi", mult *= ten);
            add("Yi", mult *= ten);
        }
    };

    template <typename Iterator>
    struct time_parser: qi::grammar<Iterator, double()>
    {
        time_parser(): time_parser::base_type(start)
        {
            multiple = (si_multiple_[qi::_val = qi::_1] | si_submultiple_[qi::_val = qi::_1] | qi::eps[qi::_val = 1]) >> qi::lit('s');
            multiple_unit = multiple[qi::_val = qi::_1] | qi::eps[qi::_val = 1];
            start = (qi::double_ >> multiple_unit)[qi::_val = qi::_1 * qi::_2];
        }

        qi::rule<Iterator, double()> start, multiple, multiple_unit;
        si_multiple si_multiple_;
        si_submultiple si_submultiple_;
    };

    template <typename Iterator>
    struct memory_parser: qi::grammar<Iterator, double()>
    {
        memory_parser(): memory_parser::base_type(start)
        {
            multiple = (binary_multiple_[qi::_val = qi::_1] | qi::eps[qi::_val = 1]) >> qi::lit('B');
            multiple_unit = (multiple[qi::_val = qi::_1] | qi::eps[qi::_val = 1]);
            start = (qi::double_ >> multiple_unit)[qi::_val = qi::_1 * qi::_2];
        }

        qi::rule<Iterator, double()> start, multiple, multiple_unit;
        binary_multiple binary_multiple_;
    };

    std::uint64_t parse_time_millis(const std::string &time)
    {
        std::string::const_iterator begin = time.begin();
        double result;
        if (!qi::parse(begin, time.end(), time_parser<std::string::const_iterator>(), result) ||
            begin != time.end())
        {
            BOOST_THROW_EXCEPTION(time_format_error() <<
                                  time_format_error::value(time) <<
                                  time_format_error::pos(begin - time.begin()));
        }
        return boost::numeric_cast<std::uint64_t>(std::round(result * 1000));
    }

    std::uint64_t parse_memory_bytes(const std::string &memory)
    {
        std::string::const_iterator begin = memory.begin();
        double result;
        if (!qi::parse(begin, memory.end(), memory_parser<std::string::const_iterator>(), result) ||
            begin != memory.end())
        {
            BOOST_THROW_EXCEPTION(memory_format_error() <<
                                  memory_format_error::value(memory) <<
                                  memory_format_error::pos(begin - memory.begin()));
        }
        return boost::numeric_cast<std::uint64_t>(std::round(result));
    }
}}}}
