#include <bacs/problem/single/driver.hpp>

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
        bunsan::filesystem::ifstream fin(location / "format");
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fin)
        {
            format.assign(std::istreambuf_iterator<char>(fin),
                          std::istreambuf_iterator<char>());
        }
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fin)
        fin.close();
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
