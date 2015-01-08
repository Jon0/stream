cmake_minimum_required (VERSION 2.6)

project (StreamServer)

set (Test_VERSION_MAJOR 0)
set (Test_VERSION_MINOR 0)

# std threads
find_package(Threads REQUIRED)

# find boost libraries
set (Boost_USE_STATIC_LIBS ON)
set (Boost_MULTITHREADED ON)
set (Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system date_time filesystem iostreams REQUIRED)
include_directories( ${Boost_INCLUDE_DIRS} )

file(GLOB SRC_FILES
	"*.cpp"
)

add_definitions(-std=c++11)
add_executable (ServerMain ${SRC_FILES})
target_link_libraries (ServerMain ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})