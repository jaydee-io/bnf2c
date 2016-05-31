################################################################################
#                                     BNF2C
#
# This file is distributed under the 4-clause Berkeley Software Distribution
# License. See LICENSE for details.
################################################################################
#.rst:
# FindBNF2C
# ---------
#
# Find BNF2C
#
# This module finds an installed BNF2C and determines the location
# of the bnf2c tool.
#
# This module sets the following result variables::
#
#   BNF2C_FOUND:      whether found BNF2C tool
#   BNF2C_EXECUTABLE: path to the bnf2c executable
#
#
# Example Usages::
#
#   find_package(BNF2C)

set(BNF2C_EXECUTABLE ${BNF2C_BUILD_PATH}/src/bnf2c)

mark_as_advanced(
    BNF2C_EXECUTABLE
)

# handle variables for found BNF2C
find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(BNF2C
    REQUIRED_VARS BNF2C_EXECUTABLE
)

function(add_parser)
    foreach(PARSER_SRC ${ARGV})
        string(REPLACE ".bnf2c" "" OUTPUT_FILE ${PARSER_SRC})

        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${BNF2C_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${PARSER_SRC} > ${OUTPUT_FILE}
            DEPENDS ${PARSER_SRC}
            COMMENT "Building parser source ${OUTPUT_FILE}"
        )
    endforeach()

    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endfunction(add_parser)
