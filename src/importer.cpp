#include "bacs/problem/single/importer.hpp"
#include "bacs/problem/single/driver.hpp"

namespace bacs{namespace problem{namespace single
{
    static const char problem_type[] = "bacs/problem/single";

    const bool importer::factory_reg_hook = importer::register_new(problem_type,
        [](const boost::property_tree::ptree &config)
        {
            const importer_ptr tmp(new importer(config));
            return tmp;
        });

    importer::importer(const boost::property_tree::ptree &config):
        m_generator(generator::instance(
            config.get<std::string>("generator.type"),
            config.get_child("generator.config", boost::property_tree::ptree()))) {}

    bacs::problem::Problem importer::convert(const options &options_)
    {
        const generator::options goptions = {
            .driver = driver::instance(options_.problem_dir),
            .destination = options_.destination,
            .root_package = options_.root_package,
        };
        bacs::problem::Problem problem_info = m_generator->generate(goptions);
        problem_info.mutable_info()->mutable_system()->set_hash(options_.hash.data(), options_.hash.size());
        problem_info.mutable_info()->mutable_system()->set_problem_type(problem_type);
        return problem_info;
    }
}}}
