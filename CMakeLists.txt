cmake_minimum_required(VERSION 3.15.0)
project(learning VERSION 0.1.0)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MYW_DISABLES "-Wno-attributes -Wno-unused-parameter")
# set(MYW_DISABLES "-Wno-attributes ")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall  ${MYW_DISABLES} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall ${MYW_DISABLES} -DNDEBUG")

# include(CTest)
# enable_testing()

add_subdirectory("lib/rltk")
include_directories("lib/rltk")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE DEBUG)
# set(CMAKE_BUILD_TYPE RELEASE)

add_subdirectory(src)
# add_subdirectory(map_view)

add_executable(
    ${PROJECT_NAME}
    "src/main.cpp"
)

target_link_libraries(${PROJECT_NAME} game tbb pthread)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
