#pragma once

#include "../tests.hpp"

#include <bacs/problem/single/detail/list_tests.hpp>
#include <bacs/problem/single/error.hpp>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
    class embedded_tests: public simple0::tests
    {
    public:
        explicit embedded_tests(
            const boost::filesystem::path &location,
            const boost::property_tree::ptree &config);

        std::unordered_set<std::string> data_set() const override
        {
            return m_tests.data_set();
        }

        std::unordered_set<std::string> test_set() const override
        {
            return m_tests.test_set();
        }

        bool make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) const
        {
            return m_tests.make_package(destination, package);
        }

    private:
        detail::list_tests m_tests;

    private:
        static const bool factory_reg_hook;
    };
}}}}}
