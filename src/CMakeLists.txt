cmake_minimum_required(VERSION 3.11)

project(wheather)

file(GLOB HEADER_FILES *.h)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  WeatherManager.cpp
  IniLoader.cpp
)

add_executable(${PROJECT_NAME} winmain.cpp ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE mswsock ws2_32)
target_compile_definitions(${PROJECT_NAME} PRIVATE -D_ENABLE_EXTENDED_ALIGNED_STORAGE)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "wheather")


target_link_libraries(${PROJECT_NAME} PRIVATE
  logger
  executor
  httpclient
  httpserver
  YanCoor
  OpenWeather
  common
  DbIntf
  in_memory_db
  postgre
  Boost::program_options
  Boost::system
#  OpenSSL::Crypto
#  OpenSSL::SSL
)

