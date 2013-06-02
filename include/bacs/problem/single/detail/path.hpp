#pragma once

#include "bacs/problem/single/settings.pb.h"

#include <boost/filesystem/path.hpp>

namespace bacs{namespace problem{namespace single{namespace detail
{
    void to_pb_path(const boost::filesystem::path &path, settings::Path &pb_path);
}}}}
