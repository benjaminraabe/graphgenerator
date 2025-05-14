cmake_minimum_required(VERSION 3.27)
project(GraphGenerator VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -fopenmp")
#set(CMAKE_BUILD_TYPE Debug)


include_directories(include)
include_directories(src)

add_executable(GraphGenerator
        main.cpp
        src/EdgeDistribution.cpp
        src/GraphModel.cpp
        src/InputModel.cpp
        src/NodeType.cpp
        src/Prime.cpp
        src/Reader.cpp
        src/Writer.cpp
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(GraphGenerator PUBLIC OpenMP::OpenMP_CXX)
endif()



# Separate testing with the Catch2-Library.
# See the repository below for further information, especially on the BSL-1.0 license, under which Catch2 is provided.
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

include_directories(tests)
add_executable(GraphGeneratorUnitTests
        tests/testPrime.cpp)
target_link_libraries(GraphGeneratorUnitTests PRIVATE Catch2::Catch2WithMain)