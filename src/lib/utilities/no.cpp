#include "bacs/single/problem/utility.hpp"

namespace bacs{namespace single{namespace problem{namespace utilities
{
    class no: public utility
    {
    public:
        no(const boost::filesystem::path &location,
           const boost::property_tree::ptree &config): utility(location, config) {}

        bool make_package(const boost::filesystem::path &/*destination*/,
                          const bunsan::pm::entry &/*package*/) override
        {
            return false;
        }

    private:
        static bool factory_reg_hook;
    };

    bool no::factory_reg_hook = utility::register_new("no",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            utility_ptr tmp(new no(location, config));
            return tmp;
        });
}}}}
