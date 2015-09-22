cmake_minimum_required(VERSION 3.0)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

project(bacs_problem_single)

if(BunsanCMakeBuiltin)
    BunsanCMakeLoad()
else()
    find_package(BunsanCMake CONFIG REQUIRED)
endif()

bunsan_find_bunsan_package(bacs_problem bacs_problem)

bunsan_add_protobuf_cxx_library(
    TARGET ${PROJECT_NAME}_pb
    PROTOS
        bacs/problem/single/intermediate.proto
        bacs/problem/single/problem.proto
        bacs/problem/single/process/execution.proto
        bacs/problem/single/process/file.proto
        bacs/problem/single/process/settings.proto
        bacs/problem/single/result.proto
        bacs/problem/single/task.proto
        bacs/problem/single/test/query.proto
        bacs/problem/single/test/sequence.proto
    LIBRARIES ${bacs_problem_LIBRARIES}
    INCLUDE_DIRECTORIES ${bacs_problem_INCLUDE_DIRS}
    INSTALL
)

bunsan_add_library(${PROJECT_NAME}
    src/driver.cpp
    src/generator.cpp

    src/test/list_storage.cpp
    src/test/matcher.cpp
    src/test/storage.cpp

    src/drivers/simple0/driver.cpp
    src/drivers/simple0/tests.cpp

    src/drivers/polygon_codeforces_com/driver.cpp
    src/drivers/polygon_codeforces_com/statement.cpp
    src/drivers/polygon_codeforces_com/tests.cpp

    src/generators/internal0.cpp

    src/importer.cpp
)
bunsan_use_target(${PROJECT_NAME} ${PROJECT_NAME}_pb)
bunsan_use_bunsan_package(${PROJECT_NAME} bacs_problem bacs_problem)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
bunsan_install_project()

bunsan_include_tests()
