cmake_minimum_required(VERSION 3.11)

project(logger)

add_library(${PROJECT_NAME} STATIC .)

target_sources(${PROJECT_NAME} PRIVATE
  src/logger.cpp
)

target_include_directories(${PROJECT_NAME} 
PRIVATE src
PUBLIC include)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::date_time)


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME} PUBLIC -Wpedantic)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE 

  )
  
