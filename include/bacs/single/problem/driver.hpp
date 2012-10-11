#pragma once

#include "bacs/single/problem/utility.hpp"

#include "bacs/single/api/pb/problem.pb.h"

#include "bunsan/factory_helper.hpp"

#include <boost/filesystem/path.hpp>

namespace bacs{namespace single{namespace problem
{
    class driver
    BUNSAN_FACTORY_BEGIN(driver, const boost::filesystem::path &/*location*/)
    public:
        static driver_ptr instance(const boost::filesystem::path &location);

    public:
        virtual api::pb::problem::Problem overview()=0;

        virtual boost::filesystem::path test(const std::string &test_id,
                                             const std::string &data_id)=0;

        // utilities
        virtual utility_ptr checker()=0;
        virtual utility_ptr validator()=0;

        /// \todo this is a stub
        virtual void *statement()=0;
    BUNSAN_FACTORY_END(driver)
}}}
