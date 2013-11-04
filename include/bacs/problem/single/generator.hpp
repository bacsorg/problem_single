#pragma once

#include <bacs/problem/single/driver.hpp>
#include <bacs/problem/single/error.hpp>

#include <bunsan/factory_helper.hpp>
#include <bunsan/pm/entry.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace problem{namespace single
{
    namespace generator_detail
    {
        struct options
        {
            driver_ptr driver;

            /// Root package directory
            boost::filesystem::path destination;

            bunsan::pm::entry root_package;
        };
    }

    struct generator_error: virtual error {};
    struct generator_generate_error: virtual generator_error
    {
        typedef boost::error_info<struct tag_options, generator_detail::options> options;
    };

    class generator: private boost::noncopyable
    BUNSAN_FACTORY_BEGIN(generator, const boost::property_tree::ptree &/*config*/)
    public:
        typedef generator_detail::options options;

    public:
        virtual Problem generate(const options &options_)=0;
    BUNSAN_FACTORY_END(generator)
}}}

namespace boost
{
    std::string to_string(const bacs::problem::single::generator_generate_error::options &options);
}
