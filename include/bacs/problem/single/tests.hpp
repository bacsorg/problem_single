#pragma once

#include <bacs/problem/utility.hpp>

#include <bunsan/factory_helper.hpp>
#include <bunsan/forward_constructor.hpp>

#include <unordered_set>

namespace bacs{namespace problem{namespace single
{
    class tests: public utility
    BUNSAN_FACTORY_BEGIN(tests, const boost::filesystem::path &/*location*/,
                                const boost::property_tree::ptree &/*config*/)
    public:
        static tests_ptr instance(const boost::filesystem::path &location);

        BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(tests, utility)

    public:
        // create() will be provided by appropriate package
        virtual std::unordered_set<std::string> data_set() const=0;
        virtual std::unordered_set<std::string> test_set() const=0;

    BUNSAN_FACTORY_END(tests)
}}}
