cmake_minimum_required(VERSION 3.23)
project(OME)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pthread -DBOOST_LOG_USE_NATIVE_SYSLOG")
set(Boost_INCLUDE_DIR D:/boost_1_80_0)
set(Boost_LIBRARY_DIR D:/boost_1_80_0/stage/lib)

include_directories(
        ${Boost_INCLUDE_DIR}
        ${Boost_LIBRARY_DIR}
)

link_directories(${Boost_LIBRARY_DIR})

set(Boost_USE_STATIC_LIBS ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS ON)  # only find release libs
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(OME main.cpp Order.cpp util.cpp logger.cpp)

target_link_libraries(OME D:/boost_1_80_0/stage/lib/libboost_serialization-mgw7-mt-sd-x64-1_80.a)
target_link_libraries(OME D:/boost_1_80_0/stage/lib/libboost_log_setup-mgw7-mt-sd-x64-1_80.a)
target_link_libraries(OME D:/boost_1_80_0/stage/lib/libboost_log-mgw7-mt-sd-x64-1_80.a)
target_link_libraries(OME D:/boost_1_80_0/stage/lib/libboost_thread-mgw7-mt-sd-x64-1_80.a)
target_link_libraries(OME D:/boost_1_80_0/stage/lib/libboost_system-mgw7-mt-sd-x64-1_80.a)
target_link_libraries(OME D:/boost_1_80_0/stage/lib/libboost_filesystem-mgw7-mt-sd-x64-1_80.a)

if (MINGW)
    target_link_libraries(OME ws2_32)
endif ()