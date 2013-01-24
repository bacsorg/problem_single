#include "bacs/single/problem/statement.hpp"
#include "bacs/single/problem/error.hpp"

#include <string>

#include <boost/filesystem/path.hpp>

namespace bacs{namespace single{namespace problem{namespace statement_versions
{
    struct copy_error: virtual error {};

    struct invalid_source_name_error: virtual copy_error
    {
        typedef boost::error_info<struct tag_source_name, boost::filesystem::path> source_name;
    };

    class copy: public statement::version
    {
    public:
        copy(const boost::filesystem::path &location,
             const boost::property_tree::ptree &config);

        void make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package,
                          const bunsan::pm::entry &resources_package) const override;

        api::pb::problem::Statement::Version info() const override;

    private:
        const std::string m_lang;
        const boost::filesystem::path m_source;

    private:
        static const bool factory_reg_hook;
    };
}}}}
