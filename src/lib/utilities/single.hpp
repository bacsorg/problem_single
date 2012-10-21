#include "bacs/single/problem/utility.hpp"

#include <string>
#include <vector>

#include <boost/optional.hpp>

namespace bacs{namespace single{namespace problem{namespace utilities
{
    class single: public utility
    {
    public:
        single(const boost::filesystem::path &location,
               const boost::property_tree::ptree &config);

        api::pb::problem::Utility info() const override;

        void make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) override;

    private:
        const boost::filesystem::path m_location;
        const boost::filesystem::path m_source;
        const boost::optional<std::string> m_std;
        const std::vector<std::string> m_libs;

    private:
        static bool factory_reg_hook;
    };
}}}}
