cmake_minimum_required(VERSION 3.27)
project(GraphGenerator VERSION 0.2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fopenmp -fwrapv")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror -Wall -Wextra -pedantic-errors -Wundef -Wstrict-overflow=5 -Wno-unused")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -fopenmp -fwrapv")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror -Wall -Wextra -pedantic-errors -Wundef -Wstrict-overflow=5 -Wno-unused")
#set(CMAKE_BUILD_TYPE Debug)


include_directories(include)

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

# Optional inclusion of OpenMP to enable multithreading.
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(GraphGenerator PUBLIC OpenMP::OpenMP_CXX)
endif()



# Optional testing with the Catch2-Library.
# See the repository below for further information, especially on the BSL-1.0 license, under which Catch2 is provided.
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)


add_executable(GraphGeneratorUnitTests
        tests/testPrime.cpp)
target_link_libraries(GraphGeneratorUnitTests PRIVATE Catch2::Catch2WithMain)