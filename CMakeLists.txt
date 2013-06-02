project(bunsan_bacs_problem_single_api)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_boost(system filesystem serialization unit_test_framework program_options thread)
bunsan_use_protobuf()

bunsan_protobuf_generate_cpp(SOURCES proto_srcs HEADERS proto_hdrs INSTALL include PROTOS
    bacs/problem/single/api/pb/task.proto
    bacs/problem/single/api/pb/result.proto
    bacs/problem/single/api/pb/resource.proto
    bacs/problem/single/api/pb/problem.proto
    bacs/problem/single/api/pb/testing.proto
    bacs/problem/single/api/pb/settings.proto
    bacs/problem/single/api/pb/intermediate.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
