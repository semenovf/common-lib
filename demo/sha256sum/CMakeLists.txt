################################################################################
# Copyright (c) 2019-2022 Vladislav Trifochkin
#
# This file is part of `common-lib`.
#
# Changelog:
#      2022.08.07 Initial version
################################################################################
project(sha256sum)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::common)
