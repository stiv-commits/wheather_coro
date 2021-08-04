cmake_minimum_required(VERSION 3.11)

project(YanCoor)

add_library(${PROJECT_NAME} STATIC .)

target_sources(${PROJECT_NAME} PRIVATE
  src/YanCoor.cpp
)

target_include_directories(${PROJECT_NAME} 
PRIVATE src
PUBLIC include)

set(CLOSE_LIBS
  logger
  httpclient
  common
  Boost::system
  ${CONAN_LIBS_OPENSSL}
)

if(WIN32)
  set(CLOSE_LIBS ${CLOSE_LIBS} mswsock ws2_32 )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${CLOSE_LIBS}
 )
  
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME} PUBLIC -Wpedantic)
endif()


