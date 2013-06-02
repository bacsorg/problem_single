#pragma once

#include "bacs/problem/single/api/pb/settings.pb.h"

#include <boost/filesystem/path.hpp>

namespace bacs{namespace problem{namespace single{namespace detail
{
    void to_pb_path(const boost::filesystem::path &path, api::pb::settings::Path &pb_path);
}}}}
