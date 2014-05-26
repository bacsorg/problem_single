#include "embedded.hpp"

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/pm/index.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <bunsan/serialization/unordered_set.hpp>

#include <unordered_map>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
    const bool embedded_tests::factory_reg_hook =
        embedded_tests::register_new("simple0_embedded",
            [](const boost::filesystem::path &location,
               const boost::property_tree::ptree &config)
            {
                tests_ptr tmp(new embedded_tests(location, config));
                return tmp;
            });

    boost::property_tree::ptree get_config(
        const boost::property_tree::ptree &config)
    {
        boost::property_tree::ptree cfg;
        cfg.put("build.builder", "simple0_embedded");
        cfg.put("utility.call", "copy");
        cfg.put_child("info", config);
        return cfg;
    }

    static const std::string data_prefix = "data_";

    embedded_tests::embedded_tests(
        const boost::filesystem::path &location,
        const boost::property_tree::ptree &config):
            tests(location, get_config(config)),
            m_tests(location, detail::list_tests::test_data_type::text)
    {
        typedef detail::list_tests::test_data test_data;
        typedef detail::list_tests::test_data_type test_data_type;

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
            m_tests.add_test(test_id, data);
        }

        // override data types from config
        for (const auto kv: config)
        {
            const std::string key = kv.second.get_value<std::string>();
            if (boost::algorithm::starts_with(key, data_prefix))
            {
                const std::string data_id = key.substr(data_prefix.size());
                const auto data_type = boost::lexical_cast<test_data_type>(
                    kv.second.get_value<std::string>()
                );
                m_tests.set_data_type(data_id, data_type);
            }
        }
    }
}}}}}
