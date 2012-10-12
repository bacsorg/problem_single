#pragma once

#include <string>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <google/protobuf/repeated_field.h>

namespace bacs{namespace single{namespace problem{namespace detail
{
    template <typename Ptree>
    std::vector<std::string> get_vector(
        const Ptree &config, const typename Ptree::path_type &name)
    {
        const boost::optional<std::string> val =
            config.template get_optional<std::string>(name);
        std::vector<std::string> vec;
        if (val)
            boost::algorithm::split(
                vec, val.get(), boost::algorithm::is_space(),
                boost::algorithm::token_compress_on);
        return vec;
    }

    template <typename Ptree>
    void parse_repeated(google::protobuf::RepeatedPtrField<std::string> &vec,
                        const Ptree &config, const typename Ptree::path_type &name)
    {
        vec.Clear();
        for (const std::string &s: get_vector(config, name))
            *vec.Add() = s;
    }
}}}}
