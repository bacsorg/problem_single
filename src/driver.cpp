#include <bacs/problem/single/driver.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>

#include <iterator>

namespace bacs{namespace problem{namespace single
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
            "bacs/problem/single" == format.substr(0, delim_pos))
        {
            return instance(format.substr(delim_pos + 1), location);
        }
        else
        {
            // TODO I am not sure that this exception is OK here.
            BOOST_THROW_EXCEPTION(unknown_driver_error() <<
                                  unknown_driver_error::factory_type(format));
        }
    }
}}}
