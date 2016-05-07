#.rst:
# FindRE2C
# ---------
#
# Find RE2C
#
# This module finds an installed RE2C and determines the location
# of the re2c tool.
#
# This module sets the following result variables::
#
#   RE2C_FOUND:      whether found RE2C tool
#   RE2C_EXECUTABLE: path to the re2c executable
#
#
# Example Usages::
#
#   find_package(RE2C)

# Try to find re2c programs
find_program(RE2C_EXECUTABLE
  NAMES re2c
)

mark_as_advanced(
    RE2C_EXECUTABLE
)

# handle variables for found RE2C
find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(RE2C
    REQUIRED_VARS RE2C_EXECUTABLE
)

function(add_lexer)
    foreach(LEXER_SRC ${ARGV})
        string(REPLACE ".re2c" "" OUTPUT_FILE ${LEXER_SRC})

        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${RE2C_EXECUTABLE} -i ${CMAKE_CURRENT_SOURCE_DIR}/${LEXER_SRC} > ${OUTPUT_FILE}
            DEPENDS ${LEXER_SRC}
            COMMENT "Building lexer source ${OUTPUT_FILE}"
        )
    endforeach()

    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endfunction(add_lexer)
