#include "bacs/single/problem/driver.hpp"

#include "bunsan/system_error.hpp"

#include <iterator>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace bacs{namespace single{namespace problem
{
    BUNSAN_FACTORY_DEFINE(driver)

    driver_ptr driver::instance(const boost::filesystem::path &location)
    {
        std::string format;
        {
            boost::filesystem::ifstream fin(location / "format");
            if (fin.bad())
                BOOST_THROW_EXCEPTION(bunsan::system_error("open"));
            format.assign(std::istreambuf_iterator<char>(fin),
                          std::istreambuf_iterator<char>());
            if (fin.bad())
                BOOST_THROW_EXCEPTION(bunsan::system_error("read"));
            fin.close();
            if (fin.bad())
                BOOST_THROW_EXCEPTION(bunsan::system_error("close"));
        }
        boost::algorithm::trim(format);
        return instance(format, location);
    }
}}}
