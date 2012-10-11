/*!
 * \file
 *
 * \brief Command line interface to BACS.SINGLE.PROBLEM.
 */

#include "yandex/contest/detail/LogHelper.hpp"
#include "yandex/contest/SystemError.hpp"
#include "yandex/contest/TypeInfo.hpp"

#include <iostream>

#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("Usage");
    try
    {
        std::vector<std::string> problems;
        desc.add_options()
            ("problem,p", po::value<std::vector<std::string>>(&problems)->composing(), "problems");
        po::positional_options_description pdesc;
        pdesc.add("problem", -1);
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(pdesc).run(), vm);
        po::notify(vm);
        for (const std::string &problem: problems)
        {
            STREAM_INFO << "Processing \"" << problem << "\" problem...";
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
