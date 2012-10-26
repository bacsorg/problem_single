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

    public:
        explicit utility(const boost::property_tree::ptree &config);

    public:
        virtual api::pb::problem::Utility info() const;

        /// One of sections in configuration.
        virtual boost::property_tree::ptree section(const std::string &name) const;

    private:
        boost::property_tree::ptree m_config;
    BUNSAN_FACTORY_END(utility)
}}}
