/*!
 * \file
 *
 * \brief Command line interface to BACS.SINGLE.PROBLEM.
 */

#include "bacs/single/problem/driver.hpp"
#include "bacs/single/problem/generator.hpp"

#include "bunsan/system_error.hpp"

#include "yandex/contest/detail/LogHelper.hpp"
#include "yandex/contest/SystemError.hpp"
#include "yandex/contest/TypeInfo.hpp"

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("Usage");
    try
    {
        std::vector<std::string> problems;
        std::string generator_type, problem_destination, problem_prefix, info_destination;
        boost::property_tree::ptree generator_config; // note: unitialized
        desc.add_options()
            ("generator,g", po::value<std::string>(&generator_type)->required(), "generator type")
            ("prefix,R", po::value<std::string>(&problem_prefix)->required(), "problems prefix")
            ("destination,d", po::value<std::string>(&problem_destination)->required(), "problems destination")
            ("info,i", po::value<std::string>(&info_destination)->required(), "info destination")
            ("problem,p", po::value<std::vector<std::string>>(&problems)->composing(), "problems");
        po::positional_options_description pdesc;
        pdesc.add("problem", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(pdesc).run(), vm);
        po::notify(vm);
        for (const boost::filesystem::path problem: problems)
        {
            STREAM_INFO << "Processing \"" << problem << "\" problem...";
            using namespace bacs::single::problem;
            namespace api = bacs::single::api;
            driver_ptr drv = driver::instance(problem);
            if (!drv)
            {
                STREAM_ERROR << "Unable to initialize driver for \"" << problem << "\" problem.";
                return 1;
            }
            const api::pb::problem::Problem info = drv->overview();
            STREAM_DEBUG << info.DebugString();
            {
                boost::filesystem::ofstream fout(info_destination / problem.filename());
                if (fout.bad())
                    BOOST_THROW_EXCEPTION(bunsan::system_error("open"));
                if (!info.SerializeToOstream(&fout))
                    BOOST_THROW_EXCEPTION(bunsan::error() << bunsan::error::message("Unable to serialize info."));
                if (fout.bad())
                    BOOST_THROW_EXCEPTION(bunsan::system_error("write"));
                fout.close();
                if (fout.bad())
                    BOOST_THROW_EXCEPTION(bunsan::system_error("close"));
            }
            generator_ptr gen = generator::instance(generator_type, generator_config);
            if (!gen)
            {
                STREAM_ERROR << "Unable to initialize generator of \"" << generator_type << "\" type.";
                return 1;
            }
            generator::options opts;
            opts.driver = drv;
            opts.destination = problem_destination / problem.filename();
            opts.root_package = bunsan::pm::entry(problem_prefix) / problem.filename().string();
            gen->generate(opts);
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
                     yandex::contest::typeinfo::name(e) << "\"." << std::endl;
        std::cerr << "what() returns the following message:" << std::endl <<
                     e.what() << std::endl;
        return 1;
    }
}
