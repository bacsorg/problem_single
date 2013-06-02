project(bunsan_bacs_problem_single)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common pm bacs_problem)
bunsan_use_boost(system filesystem serialization unit_test_framework program_options thread)
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

    src/lib/tests.cpp
    src/lib/driver.cpp
    src/lib/generator.cpp

    src/lib/detail/path.cpp

    src/lib/drivers/simple0.cpp
    src/lib/drivers/simple0_tests.cpp

    src/lib/generators/internal0.cpp
)
bunsan_use(${PROJECT_NAME})

bunsan_add_executable(${PROJECT_NAME}_cli
    src/bin/cli.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_cli)

bunsan_include_tests()
