#pragma once

#include "bacs/single/problem/utility.hpp"
#include "bacs/single/problem/tests.hpp"
#include "bacs/single/problem/statement.hpp"

#include "bacs/single/api/pb/problem.pb.h"

#include "bunsan/factory_helper.hpp"

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

namespace bacs{namespace single{namespace problem
{
    class driver: private boost::noncopyable
    BUNSAN_FACTORY_BEGIN(driver, const boost::filesystem::path &/*location*/)
    public:
        static driver_ptr instance(const boost::filesystem::path &location);

    public:
        /*!
         * \warning package names are relative to problem root package,
         * i.e. they should be prepended with root entry for problem,
         * e.g. "tests" package should become "bacs/problem/1000/tests",
         * where "bacs/problem/1000" corresponds problem
         *
         * \warning package name itself should be initialized later
         *
         * \warning hash should be initialized later
         */
        virtual api::pb::problem::Problem overview() const=0;

        // utilities
        virtual tests_ptr tests() const=0;

        virtual utility_ptr checker() const=0;

        /// \return nullptr if not provided
        virtual utility_ptr validator() const=0;

        virtual statement_ptr statement() const=0;
    BUNSAN_FACTORY_END(driver)
}}}
