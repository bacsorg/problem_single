#include <bacs/problem/single/generator.hpp>

#include <sstream>

BUNSAN_FACTORY_DEFINE(bacs::problem::single::generator)

namespace boost
{
    std::string to_string(
        const bacs::problem::single::generator_generate_error::options &options)
    {
        std::ostringstream sout;
        sout << "[" << bunsan::error::info_name(options) << "] = { ";
        sout << "destination = " << options.value().destination << ", ";
        sout << "root_package = " << options.value().root_package;
        sout << " }\n";
        return sout.str();
    }
}
