#pragma once

#include "bacs/single/problem/utility.hpp"

namespace bacs{namespace single{namespace problem{namespace drivers
{
    // implementation without registration in factory
    class simple0_tests: public utility
    {
    public:
        explicit simple0_tests(const boost::filesystem::path &location);

        void make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) override;

    private:
        const boost::filesystem::path m_location;
    };
}}}}
