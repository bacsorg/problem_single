#include "bacs/single/problem/driver.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"

#include <iterator>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>

namespace bacs{namespace single{namespace problem
{
    BUNSAN_FACTORY_DEFINE(driver)

    driver_ptr driver::instance(const boost::filesystem::path &location)
    {
        std::string format;
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ifstream fin(location / "format");
            format.assign(std::istreambuf_iterator<char>(fin),
                          std::istreambuf_iterator<char>());
            fin.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
        boost::algorithm::trim(format);
        return instance(format, location);
    }
}}}
