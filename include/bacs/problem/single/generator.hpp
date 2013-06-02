#pragma once

#include "bacs/problem/single/driver.hpp"

#include "bunsan/factory_helper.hpp"
#include "bunsan/pm/entry.hpp"

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace problem{namespace single
{
    class generator: private boost::noncopyable
    BUNSAN_FACTORY_BEGIN(generator, const boost::property_tree::ptree &/*config*/)
    public:
        struct options
        {
            driver_ptr driver;

            /// Root package directory
            boost::filesystem::path destination;

            bunsan::pm::entry root_package;
        };

    public:
        virtual api::pb::problem::Problem generate(const options &options_)=0;
    BUNSAN_FACTORY_END(generator)
}}}
