include_directories(./ ./EncodeTool)

include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

include_directories(/usr/include)
include_directories(/usr/include/mysql)
link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS system thread)
#find_package(Boost COMPONENTS system filesystem REQUIRED)
FIND_PACKAGE(PCL 1.8 REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

find_package(OpenCV REQUIRED)

aux_source_directory(. DIR_CONNECT_SRCS)
add_library(Connect ${DIR_CONNECT_SRCS})
aux_source_directory(. DIR_ENCODETOOL_SRCS)
add_library(EncodeTool ${DIR_ENCODETOOL_SRCS})
target_link_libraries(Connect ${PCL_LIBRARIES} ${LIBMYSQLCLIENT_LIBRARIES} ${OpenCV_LIBS} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} EncodeTool)

