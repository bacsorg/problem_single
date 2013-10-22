project(bunsan_bacs_problem_single)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common pm bacs_problem)
bunsan_use_boost(system filesystem serialization program_options thread)
bunsan_use_protobuf()

bunsan_protobuf_generate_cpp(SOURCES proto_srcs HEADERS proto_hdrs INSTALL include PROTOS
    bacs/problem/single/task.proto
    bacs/problem/single/result.proto
    bacs/problem/single/resource.proto
    bacs/problem/single/problem.proto
    bacs/problem/single/testing.proto
    bacs/problem/single/settings.proto
    bacs/problem/single/intermediate.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}

    src/tests.cpp
    src/driver.cpp
    src/generator.cpp

    src/detail/path.cpp
    src/detail/list_tests.cpp

    src/drivers/simple0.cpp
    src/drivers/simple0_tests.cpp

    src/generators/internal0.cpp

    src/importer.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
