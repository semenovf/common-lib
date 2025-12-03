################################################################################
# Copyright (c) 2025 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2025.12.02 Initial version.
################################################################################
project(termcolor-demo)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::common)

