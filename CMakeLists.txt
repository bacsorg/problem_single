project(bunsan_bacs_single_api)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
bunsan_use(${PROTOBUF_LIBRARIES})

bunsan_use_boost(system filesystem serialization unit_test_framework program_options thread)

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
    proto/intermediate.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}
)

#bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})
install(FILES ${proto_hdrs} DESTINATION include/bacs/single/api/pb)

bunsan_include_tests()
