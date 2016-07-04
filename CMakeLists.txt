cmake_minimum_required(VERSION 2.8)
project(kcf)

find_package(OpenCV REQUIRED)

ADD_DEFINITIONS("-std=c++0x")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
include_directories(src) 
FILE(GLOB_RECURSE sourcefiles "src/*.cpp")
add_executable( kcf ${sourcefiles} )
target_link_libraries( kcf ${OpenCV_LIBS})
