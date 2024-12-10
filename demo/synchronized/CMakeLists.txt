################################################################################
# Copyright (c) 2024 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2024.12.10 Initial version.
################################################################################
project(synchronized-demo)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::common)

