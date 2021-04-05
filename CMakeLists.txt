cmake_minimum_required(VERSION 3.15.0)
project(learning VERSION 0.1.0)

# set(BUILD_SHARED_LIBS ON)
# set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
find_package(spdlog REQUIRED)

set(MYW_DISABLES "-Wextra -Wno-attributes -Wno-unused-parameter  -Wno-ignored-qualifiers")
# set(MYW_DISABLES "-Wno-attributes ")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -Wall ${MYW_DISABLES} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}-O2 -Wall ${MYW_DISABLES} ")
# set(CMAKE_BUILD_TYPE DEBUG)
# set(CMAKE_BUILD_TYPE RELEASE)

# include(CTest)
# enable_testing()



add_subdirectory("lib/rltk")
include_directories("lib/rltk")

set(CMAKE_CXX_STANDARD 20)

set(USER_CODE_FLAGS "-fdiagnostics-color=always ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${USER_CODE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${USER_CODE_FLAGS}")

include_directories("./src")
# add_subdirectory(src)
add_subdirectory("./src/core")
add_subdirectory("./src/utils")
add_subdirectory("./src/system")
add_subdirectory("./src/component")
# add_subdirectory(map_view)

add_executable(
    ${PROJECT_NAME}
    "src/core/main.cpp"
)

target_link_libraries(${PROJECT_NAME} game_core tbb pthread)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
