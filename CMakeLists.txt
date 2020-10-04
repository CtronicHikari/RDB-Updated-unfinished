cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(manager)

aux_source_directory(. DIRSRCS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

find_package(PCL 1.8 REQUIRED)
find_package(OpenCV REQUIRED) 
find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(/usr/include)
include_directories(/usr/include/mysql)
include_directories(./ ./Connect)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(Connect)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (manager ${DIRSRCS})

target_link_libraries (manager ${PCL_LIBRARIES} ${OpenCV_LIBS} ${LIBMYSQLCLIENT_LIBRARIES} ${Boost_LIBRARIES} Connect)

