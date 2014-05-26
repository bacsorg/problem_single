#include "tests.hpp"

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
    tests_ptr tests::instance(
        const boost::filesystem::path &location,
        const boost::property_tree::ptree &config)
    {
        const std::string type = config.get<std::string>("type", "simple0_embedded");
        return instance(type, location, config);
    }
}}}}}
