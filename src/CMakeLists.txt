cmake_minimum_required(VERSION 3.5.1)
project(cpp)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    link_directories(Z3PATH/build)
    set(Z3_FILE libz3.so)
    ADD_DEFINITIONS(-DLINUX)
else()
    link_directories(Z3PATH/build)
    set(Z3_FILE libz3.dylib)
endif()


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(. ./basic ./solver ./parser)
INCLUDE_DIRECTORIES(GUROBIPATH/include)
INCLUDE_DIRECTORIES(Z3PATH/src/api/c++)
INCLUDE_DIRECTORIES(Z3PATH/src/api)

ADD_DEFINITIONS(-DDEBUG)

file(GLOB basic_files "basic/*.h" "basic/*.cpp")
file(GLOB parser_files "parser/*.h" "parser/*.cpp")
file(GLOB solver_files "solver/*.h" "solver/*.cpp")
add_library(basic_lib ${basic_files})
add_library(parser_lib ${parser_files})
add_library(solver_lib ${solver_files})

INCLUDE(FindPkgConfig)
find_package(Jsoncpp)
include_directories(${Jsoncpp_INCLUDE_DIR})
find_package(PROFILER)
include_directories(${LIBPROFILER_INCLUDE_DIRS})
link_directories(GUROBIPATH/lib/)

add_executable(run main/main.cpp)
target_link_libraries(run parser_lib solver_lib parser_lib basic_lib ${Jsoncpp_LIBRARY} ${LIBPROFILER_LIBRARIES} glog gflags libgurobi_g++5.2.a libgurobi91.so ${Z3_FILE})
