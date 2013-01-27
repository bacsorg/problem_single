#pragma once

#include "bacs/single/problem/buildable.hpp"

#include "bacs/single/api/pb/problem.pb.h"

#include "bunsan/factory_helper.hpp"
#include "bunsan/forward_constructor.hpp"

#include <unordered_set>

#include <boost/filesystem/path.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace bacs{namespace single{namespace problem
{
    class statement: public buildable
    {
    public:
        class version: private boost::noncopyable
        BUNSAN_FACTORY_BEGIN(version, const boost::filesystem::path &/*location*/,
                                      const boost::property_tree::ptree &/*config*/)
        public:
            struct manifest
            {
                template <typename Archive>
                void serialize(Archive &ar, const unsigned int)
                {
                    ar & BOOST_SERIALIZATION_NVP(version);
                    ar & BOOST_SERIALIZATION_NVP(data);
                }

                struct
                {
                    template <typename Archive>
                    void serialize(Archive &ar, const unsigned int)
                    {
                        ar & BOOST_SERIALIZATION_NVP(lang);
                        ar & BOOST_SERIALIZATION_NVP(format);
                    }

                    std::string lang;
                    std::string format;
                } version;

                struct
                {
                    template <typename Archive>
                    void serialize(Archive &ar, const unsigned int)
                    {
                        ar & BOOST_SERIALIZATION_NVP(index);
                    }

                    boost::filesystem::path index;
                } data;
            };

        public:
            /// \note config_location.parent_path() is location
            static version_ptr instance(const boost::filesystem::path &config_location);

        public:
            version(const std::string &lang_, const std::string &format_);

            virtual ~version();

            virtual void make_package(const boost::filesystem::path &destination,
                                      const bunsan::pm::entry &package,
                                      const bunsan::pm::entry &resources_package) const=0;

            /// \warning package name is relative to statement version package
            virtual api::pb::problem::Statement::Version info() const=0;

        public:
            virtual std::string lang() const;
            virtual std::string format() const;

            /// relative unique entry name for this statement version
            virtual bunsan::pm::entry subpackage() const;

        private:
            std::string m_lang;
            std::string m_format;

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
