project(bunsan_bacs_problem_single)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 3.0)

find_package(BunsanCMake CONFIG REQUIRED)

bunsan_find_bunsan_package(bunsan_bacs_problem bunsan_bacs_problem)

bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}_pb
    SHARED
    PROTOS
        bacs/problem/single/task.proto
        bacs/problem/single/result.proto
        bacs/problem/single/problem.proto
        bacs/problem/single/testing.proto
        bacs/problem/single/settings.proto
        bacs/problem/single/intermediate.proto
    LIBRARIES ${bunsan_bacs_problem_LIBRARIES}
    INCLUDE_DIRECTORIES ${bunsan_bacs_problem_INCLUDE_DIRS}
    INSTALL
)

bunsan_add_shared_library(${PROJECT_NAME}
    src/tests.cpp
    src/driver.cpp
    src/generator.cpp

    src/detail/path.cpp
    src/detail/list_tests.cpp

    src/drivers/simple0/driver.cpp
    src/drivers/simple0/tests.cpp

    src/drivers/polygon_codeforces_com/driver.cpp
    src/drivers/polygon_codeforces_com/statement.cpp
    src/drivers/polygon_codeforces_com/tests.cpp

    src/generators/internal0.cpp

    src/importer.cpp
)
bunsan_use_target(${PROJECT_NAME} ${PROJECT_NAME}_pb)
bunsan_use_bunsan_package(${PROJECT_NAME} bunsan_bacs_problem bunsan_bacs_problem)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
