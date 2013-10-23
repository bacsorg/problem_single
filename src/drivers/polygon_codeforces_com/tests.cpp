#include "tests.hpp"

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace polygon_codeforces_com
{
    tests::tests(const boost::filesystem::path &location):
        list_tests(
            location,
            list_tests::test_data_type::text,
            "[polygon_codeforces_com tests generator]") {}

    void tests::add_test(const std::string &test_id,
                         const boost::filesystem::path &in,
                         const boost::filesystem::path &out)
    {
        list_tests::add_test(test_id, {
            {"in", in},
            {"out", out},
        });
    }
}}}}}
