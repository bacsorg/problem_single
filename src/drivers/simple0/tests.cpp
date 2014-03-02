#include "tests.hpp"

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/pm/index.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <bunsan/serialization/unordered_set.hpp>

#include <unordered_map>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
    tests::tests(const boost::filesystem::path &location):
        list_tests(
            location,
            list_tests::test_data_type::text,
            "[simple0 tests generator]")
    {
        std::unordered_map<std::string, std::unordered_set<std::string>> test_files;
        for (boost::filesystem::directory_iterator i(location), end; i != end; ++i)
        {
            if (!boost::filesystem::is_regular_file(i->path()))
                BOOST_THROW_EXCEPTION(test_format_error());
            const std::string fname = i->path().filename().string();
            const std::string::size_type dot = fname.find('.');
            if (dot == std::string::npos)
                BOOST_THROW_EXCEPTION(test_format_error());
            if (fname.find('.', dot + 1) != std::string::npos)
                BOOST_THROW_EXCEPTION(test_format_error());
            const std::string test_id = fname.substr(0, dot);
            const std::string data_id = fname.substr(dot + 1);
            test_files[test_id].insert(data_id);
        }
        for (const auto &id_data: test_files)
        {
            const std::string &test_id = id_data.first;
            test_data data;
            for (const auto &data_id: id_data.second)
                data[data_id] = test_id + "." + data_id;
            add_test(test_id, data);
        }
        // simple0-related restriction
        const auto data_set_ = data_set();
        if (data_set_.find("in") == data_set_.end())
            BOOST_THROW_EXCEPTION(test_no_in_data_error());
        // note out is optional
        const std::size_t expected_size =
            data_set_.find("out") == data_set_.end() ? 1 : 2;
        if (data_set_.size() != expected_size)
            // TODO send at least first unknown data_id
            BOOST_THROW_EXCEPTION(test_unknown_data_error());
    }
}}}}}
