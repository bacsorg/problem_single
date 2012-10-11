project(bunsan_bacs_single_problem)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(YandexContestCommon)

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options thread REQUIRED)
list(APPEND includes ${Boost_INCLUDE_DIRS})
list(APPEND libraries ${Boost_LIBRARIES})

# Local
include_directories(include ${includes})

# bacs::single::api
list(APPEND libraries bunsan_bacs_single_api)

# yandex_contest_(
list(APPEND libraries
    yandex_contest_common
    yandex_contest_system
)

# Targets
add_library(${PROJECT_NAME} SHARED
    src/lib/buildable.cpp
    src/lib/utility.cpp

    src/lib/utilities/single.cpp
)
target_link_libraries(${PROJECT_NAME} ${libraries})

add_executable(${PROJECT_NAME}_cli src/bin/cli.cpp)
target_link_libraries(${PROJECT_NAME}_cli ${PROJECT_NAME} ${libraries})

# Installation
install(DIRECTORY include DESTINATION .)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

yandex_contest_tests()
