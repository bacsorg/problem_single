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
        constexpr char delim = '#';
        const std::string::size_type delim_pos = format.find(delim);
        if (format.find(delim, delim_pos + 1) == std::string::npos &&
            delim_pos != std::string::npos &&
            "bacs/single/problem" == format.substr(0, delim_pos))
        {
            return instance(format.substr(delim_pos + 1), location);
        }
        else
        {
            return driver_ptr();
        }
    }
}}}
