enable_testing()

macro(add_library_unittest unittest_name)
    # Add test binary
    include_directories(${CMAKE_SOURCE_DIR}/utils/unittest/googletest/include)
    add_executable(${unittest_name} ${ARGN})
    target_link_libraries(${unittest_name} gtest gtest_main)
	add_dependencies(${unittest_name} bnf2c)

    # Add test to test suite
    add_test(NAME ${unittest_name} COMMAND ${unittest_name} --gtest_color=true)

    # Add a target to launch the test
    set(runtest_name RunTest-${unittest_name})
    set_property(GLOBAL APPEND PROPERTY UNIT_TESTS_LIST ${runtest_name})
    add_custom_target(${runtest_name}
        COMMAND ${CMAKE_CTEST_COMMAND} -V ${unittest_name}
        DEPENDS ${unittest_name}
    )
endmacro(add_library_unittest)
