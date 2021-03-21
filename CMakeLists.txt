cmake_minimum_required(VERSION 3.15.0)
project(learning VERSION 0.1.0)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(MYW_DISABLES "-Wno-attributes ")
set(MYW_DISABLES "-Wno-attributes -Wno-unused-parameter")

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -O0 -Wall -Wextra ${MYW_DISABLES} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Wextra ${MYW_DISABLES} -DNDEBUG")


# include(CTest)
# enable_testing()


add_subdirectory("lib/libtcod")

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(src)

# set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)



add_executable(
    ${PROJECT_NAME}
    "src/main.cpp"
)

target_link_libraries(${PROJECT_NAME} game tbb pthread)


# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
