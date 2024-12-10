################################################################################
# Copyright (c) 2019-2023 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2023.07.17 Initial version.
################################################################################
project(standard_paths-demo)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::common)

