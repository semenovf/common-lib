################################################################################
# Copyright (c) 2019-2023 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2023.07.16 Initial version.
################################################################################
project(traverse_directory)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::common)

