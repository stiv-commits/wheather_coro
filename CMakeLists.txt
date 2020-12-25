cmake_minimum_required(VERSION 3.11)

project(wheather_server)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/scripts")
set(3RD "${CMAKE_SOURCE_DIR}/third_party")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/scripts")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(WIN32)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  add_definitions(-DWIN32 -D_WIN32 -DWINDOWS -D_WINDOWS -D_WIN32_WINNT=0x0601 -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
else()
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++2a -lpthread -lrt -ldl -Wno-deprecated-declarations")
endif()


# ------------- BOOST ------------
#set(Boost_DEBUG 1)
option(ENABLE_BOOST "Enable boost libraries" ON)
option(Boost_NO_BOOST_CMAKE ON)
set( ENV{Boost_DIR}       "${CMAKE_SOURCE_DIR}/third_party/boost" )

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.75.0 REQUIRED COMPONENTS 
	program_options system log date_time thread)	
add_definitions(-D__STDC_FORMAT_MACROS -DBOOST_ALL_NO_LIB -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS 
  -DBOOST_MPL_LIMIT_LIST_SIZE=50
  -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# ------------- OPENSSL ------------
find_package(OpenSSL REQUIRED)

# ------------- PROJECT  ------------
add_subdirectory(lib)
add_subdirectory(src)
