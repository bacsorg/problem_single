project(bunsan_bacs_single_problem)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_bunsan(common pm bacs_single_api)
bunsan_use_boost(system filesystem serialization unit_test_framework program_options thread)
bunsan_use_protobuf()

bunsan_add_shared_library(${PROJECT_NAME}
    src/lib/buildable.cpp
    src/lib/utility.cpp
    src/lib/tests.cpp
    src/lib/statement.cpp
    src/lib/driver.cpp
    src/lib/generator.cpp

    src/lib/detail/path.cpp
    src/lib/detail/resource.cpp

    src/lib/utilities/none.cpp
    src/lib/utilities/single.cpp

    src/lib/statement_versions/copy.cpp
    src/lib/statement_versions/pdflatex.cpp

    src/lib/drivers/simple0.cpp
    src/lib/drivers/simple0_tests.cpp

    src/lib/generators/internal0.cpp
)
list(APPEND libraries ${PROJECT_NAME})

bunsan_add_executable(${PROJECT_NAME}_cli
    src/bin/cli.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_cli)

bunsan_include_tests()
