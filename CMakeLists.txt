cmake_minimum_required(VERSION 3.15.0)
project(learning VERSION 0.1.0)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -O0 -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")


# include(CTest)
# enable_testing()

# add_subdirectory(src)
add_subdirectory("lib/libtcod")
add_subdirectory("src")

include_directories("src/include")
# include_directories("lib/libtcod/src")
add_executable(
    ${PROJECT_NAME}
    "src/main.cpp"
)

target_link_libraries(${PROJECT_NAME} pthread game)

# set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BUILD_TYPE DEBUG)
# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
