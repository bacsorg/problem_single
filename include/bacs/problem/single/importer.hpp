#pragma once

#include <bacs/problem/single/generator.hpp>

#include <bacs/problem/importer.hpp>

namespace bacs{namespace problem{namespace single
{
    class importer: public bacs::problem::importer
    {
    public:
        explicit importer(const boost::property_tree::ptree &config);

        bacs::problem::Problem convert(const options &options_) override;

    private:
        const generator_ptr m_generator;

    private:
        static const bool factory_reg_hook;
    };
}}}
