#include "bacs/single/problem/driver.hpp"

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
            fin.exceptions(std::ios::badbit);
            format.assign(std::istreambuf_iterator<char>(fin),
                          std::istreambuf_iterator<char>());
            fin.close();
        }
        boost::algorithm::trim(format);
        return instance(format, location);
    }
}}}
