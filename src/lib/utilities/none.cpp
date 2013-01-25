#include "bacs/single/problem/utility.hpp"

namespace bacs{namespace single{namespace problem{namespace utilities
{
    class none: public utility
    {
    public:
        none(const boost::filesystem::path &location,
             const boost::property_tree::ptree &config): utility(location, config) {}

        bool make_package(const boost::filesystem::path &/*destination*/,
                          const bunsan::pm::entry &/*package*/) const override
        {
            return false;
        }

    private:
        static const bool factory_reg_hook;
    };

    const bool none::factory_reg_hook = utility::register_new("none",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            utility_ptr tmp(new none(location, config));
            return tmp;
        });
}}}}
