#pragma once

#include "bacs/single/problem/utility.hpp"

#include "bunsan/factory_helper.hpp"

#include <unordered_set>
#include <utility>

namespace bacs{namespace single{namespace problem
{
    class tests: public utility
    BUNSAN_FACTORY_BEGIN(tests, const boost::filesystem::path &/*location*/,
                                const boost::property_tree::ptree &/*config*/)
    public:
        static tests_ptr instance(const boost::filesystem::path &location);

        template <typename ... Args>
        explicit tests(Args &&...args): utility(std::forward<Args>(args)...) {}

    public:
        // create() will be provided by appropriate package
        virtual std::unordered_set<std::string> data_set()=0;
        virtual std::unordered_set<std::string> test_set()=0;

    BUNSAN_FACTORY_END(tests)
}}}
