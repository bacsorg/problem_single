project(bunsan_bacs_api_tests)

cmake_minimum_required(VERSION 2.8)

message("tests were included")

aux_source_directory(. test_srcs)

foreach(src ${test_srcs})
    string(REGEX REPLACE "^.*/([^/]+)\\.cpp$" "\\1" trgt ${src})
    add_executable(test_${trgt} ${src})
    target_link_libraries(test_${trgt} bunasn_bacs_api ${libraries})
    add_test(${trgt} test_${trgt})
endforeach()

