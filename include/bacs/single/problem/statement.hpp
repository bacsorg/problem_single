#pragma once

#include "bacs/single/problem/buildable.hpp"

#include "bacs/single/api/pb/problem.pb.h"

#include "bunsan/factory_helper.hpp"
#include "bunsan/forward_constructor.hpp"

#include <unordered_set>

namespace bacs{namespace single{namespace problem
{
    class statement: public buildable
    {
    public:
        class version: private boost::noncopyable
        BUNSAN_FACTORY_BEGIN(version, const boost::filesystem::path &/*location*/,
                                      const boost::property_tree::ptree &/*config*/)
        public:
            /// \note config_location.parent_path() is location
            static version_ptr instance(const boost::filesystem::path &config_location);

        public:
            virtual ~version();

            virtual void make_package(const boost::filesystem::path &destination,
                                      const bunsan::pm::entry &package,
                                      const bunsan::pm::entry &resources_package) const=0;

            /// \warning package name is relative to statement version package
            virtual api::pb::problem::Statement::Version info() const=0;

        BUNSAN_FACTORY_END(version)

        typedef std::shared_ptr<statement> statement_ptr;

    public:
        static statement_ptr instance(const boost::filesystem::path &location);

    private:
        explicit statement(const boost::filesystem::path &location);

    public:
        bool make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) const override;

        /// \warning package names are relative to statement package
        const api::pb::problem::Statement &info() const;

    private:
        const boost::filesystem::path m_location;
        std::vector<version_ptr> m_versions;
        api::pb::problem::Statement m_info;
    };
    typedef statement::statement_ptr statement_ptr;
}}}
