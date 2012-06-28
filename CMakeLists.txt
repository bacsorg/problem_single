project(bunsan_bacs_api)

cmake_minimum_required(VERSION 2.8)

if(${CMAKE_COMPILER_IS_GNUCXX})
    set(gxx_flags "-std=c++0x -Wall -Wextra")
    if(${UNIX})
        set(gxx_flags "-pthread ${gxx_flags}")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "-g ${gxx_flags}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 ${gxx_flags}")
endif()

include(${CMAKE_SOURCE_DIR}/user-config.cmake OPTIONAL)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

if(NOT DEFINED ENABLE_TESTS)
    set(ENABLE_TESTS ON CACHE BOOL "Do you want to enable testing?" FORCE)
endif()

# BOOST
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem serialization iostreams unit_test_framework program_options thread REQUIRED)
list(APPEND includes ${Boost_INCLUDE_DIRS})
list(APPEND libraries ${Boost_LIBRARIES})

# ProtoBuf

find_package(Protobuf REQUIRED)

# bacs/api/pb includes
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/bacs)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/api)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/pb)

# find all proto files
file(GLOB proto proto/*.proto)

# compile proto files
set(PROTOBUF_IMPORT_DIRS proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(proto_srcs proto_hdrs ${proto})
list(APPEND includes ${PROTOBUF_INCLUDE_DIRS})
list(APPEND libraries ${PROTOBUF_LIBRARIES})

# sources

aux_source_directory(src srcs)

include_directories(include ${includes})

add_library(${PROJECT_NAME} SHARED ${srcs} ${proto_srcs})
target_link_libraries(${PROJECT_NAME} ${libraries})

# install

install(DIRECTORY include DESTINATION .)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(FILES ${proto_hdrs} DESTINATION include/bacs/api/pb)

# testing

if(${ENABLE_TESTS})
    enable_testing()
    add_subdirectory(tests ${PROJECT_BINARY_DIR}/tests)
endif()

