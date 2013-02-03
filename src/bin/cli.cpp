/*!
 * \file
 *
 * \brief Command line interface to BACS.SINGLE.PROBLEM.
 */

#include "bacs/single/problem/driver.hpp"
#include "bacs/single/problem/generator.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/logging/legacy.hpp"
#include "bunsan/filesystem/fstream.hpp"
#include "bunsan/runtime/demangle.hpp"

#include <iostream>

#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("Usage");
    try
    {
        std::vector<std::string> problems;
        std::string generator_type, repository, problem_prefix, info_destination;
        boost::property_tree::ptree generator_config; // note: unitialized
        desc.add_options()
            ("generator,g", po::value<std::string>(&generator_type)->required(), "generator type")
            ("prefix,R", po::value<std::string>(&problem_prefix)->required(), "problems prefix")
            ("repository,p", po::value<std::string>(&repository)->required(), "problems destination")
            ("info,i", po::value<std::string>(&info_destination)->required(), "info destination")
            ("problem", po::value<std::vector<std::string>>(&problems)->composing(), "problems");
        po::positional_options_description pdesc;
        pdesc.add("problem", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(pdesc).run(), vm);
        po::notify(vm);
        for (const boost::filesystem::path problem: problems)
        {
            SLOG("Processing " << problem.filename() << " problem at " << problem << "...");
            const boost::filesystem::path qproblem = problem_prefix / problem.filename();
            const boost::filesystem::path dproblem = repository / qproblem;
            const boost::filesystem::path iproblem = info_destination / problem.filename();
            using namespace bacs::single::problem;
            namespace api = bacs::single::api;
            const driver_ptr drv = driver::instance(problem);
            const generator_ptr gen = generator::instance(generator_type, generator_config);
            generator::options opts;
            opts.driver = drv;
            opts.destination = dproblem;
            opts.root_package = qproblem.string();
            const api::pb::problem::Problem info = gen->generate(opts);
            SLOG(info.DebugString());
            BUNSAN_EXCEPTIONS_WRAP_BEGIN()
            {
                bunsan::filesystem::ofstream fout(iproblem);
                if (!info.SerializeToOstream(&fout))
                    BOOST_THROW_EXCEPTION(bunsan::system_error("write") << bunsan::error::message("Unable to serialize info."));
                fout.close();
            }
            BUNSAN_EXCEPTIONS_WRAP_END()
        }
    }
    catch (po::error &e)
    {
        std::cerr << e.what() << std::endl << std::endl << desc << std::endl;
        return 200;
    }
    catch (std::exception &e)
    {
        std::cerr << "Program terminated due to exception of type \"" <<
                     bunsan::runtime::type_name(e) << "\"." << std::endl;
        std::cerr << "what() returns the following message:" << std::endl <<
                     e.what() << std::endl;
        return 1;
    }
}
