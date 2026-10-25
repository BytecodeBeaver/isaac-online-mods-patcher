cmake_minimum_required(VERSION 3.20)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/TargetPlatform.cmake)

project(IsaacOnlineModsBypass)

set(CMAKE_CXX_STANDARD 20)

add_executable(IsaacOnlineModsBypass
        main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "isaac-online-mods-patcher")
