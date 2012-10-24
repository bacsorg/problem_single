project(bunsan_bacs_single_api)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(YandexContestCommon)

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options thread REQUIRED)
list(APPEND includes ${Boost_INCLUDE_DIRS})
list(APPEND libraries ${Boost_LIBRARIES})

# ProtoBuf
find_package(Protobuf REQUIRED)
list(APPEND includes ${PROTOBUF_INCLUDE_DIRS})
list(APPEND libraries ${PROTOBUF_LIBRARIES})

# bacs/api/pb includes
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/bacs)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/single)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/api)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/pb)

# Compile proto files
set(PROTOBUF_IMPORT_DIRS proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(proto_srcs proto_hdrs
    proto/task.proto
    proto/result.proto
    proto/resource.proto
    proto/problem.proto
    proto/testing.proto
    proto/settings.proto
)

# Local
include_directories(include ${includes})

# Targets
add_library(${PROJECT_NAME} SHARED
    ${srcs}
    ${proto_srcs}
)
target_link_libraries(${PROJECT_NAME} ${libraries})

# Installation
#install(DIRECTORY include DESTINATION .)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(FILES ${proto_hdrs} DESTINATION include/bacs/single/api/pb)

yandex_contest_tests()
