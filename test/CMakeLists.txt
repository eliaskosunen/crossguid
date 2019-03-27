execute_process(COMMAND git submodule update --init -- doctest
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tests
    test.cpp test_main.cpp test_empty.cpp)
target_link_libraries(tests PRIVATE crossguid)
target_include_directories(tests SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/doctest/doctest)
set_private_flags(tests)
add_test(NAME tests COMMAND tests)
