#pragma once

#include "bacs/single/problem/buildable.hpp"

#include "bunsan/factory_helper.hpp"

#include "bacs/single/api/pb/problem.pb.h"

#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace single{namespace problem
{
    class utility: public buildable
    BUNSAN_FACTORY_BEGIN(utility, const boost::filesystem::path &/*location*/,
                                  const boost::property_tree::ptree &/*config*/)
    public:
        static utility_ptr instance(const boost::filesystem::path &location);
        static utility_ptr instance_optional(const boost::filesystem::path &location);

    public:
        utility(const boost::filesystem::path &location,
                const boost::property_tree::ptree &config);

    public:
        virtual api::pb::problem::Utility info() const;

        /// One of sections in configuration.
        virtual boost::property_tree::ptree section(const std::string &name) const;

        virtual boost::filesystem::path location() const;

        virtual boost::filesystem::path target() const;

    private:
        const boost::filesystem::path m_location;
        const boost::filesystem::path m_target;
        const boost::property_tree::ptree m_config;
    BUNSAN_FACTORY_END(utility)
}}}
