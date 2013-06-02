#pragma once

#include "bacs/problem/single/pb/settings.pb.h"

#include <boost/filesystem/path.hpp>

namespace bacs{namespace problem{namespace single{namespace detail
{
    void to_pb_path(const boost::filesystem::path &path, pb::settings::Path &pb_path);
}}}}
