cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTRA_COMPONENT_DIRS libs)

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED OFF)
##set(Boost_USE_STATIC_RUNTIME ON)
#find_package(Boost 1.65 COMPONENTS core)
#
#if(Boost_FOUND)
##    include_directories(${Boost_INCLUDE_DIRS})
##    target_link_libraries(${IDF_TARGET} ${Boost_LIBRARIES})
#endif()

project(smartscreen)
