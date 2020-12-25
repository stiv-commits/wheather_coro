cmake_minimum_required(VERSION 3.11)

project(wheather)

file(GLOB HEADER_FILES *.h)
source_group("Header Files" FILES ${HEADER_FILES})

set(SOURCE_FILES
  WeatherManager.cpp
  IniLoader.cpp
)

if(WIN32)
  add_executable(${PROJECT_NAME} winmain.cpp ${SOURCE_FILES} ${HEADER_FILES})
  target_link_libraries(${PROJECT_NAME} PRIVATE mswsock ws2_32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE -D_ENABLE_EXTENDED_ALIGNED_STORAGE)
else()
  add_executable(${PROJECT_NAME} linmain.cpp ${SOURCE_FILES} ${HEADER_FILES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "wheather")


target_link_libraries(${PROJECT_NAME} PRIVATE


  Boost::program_options
  Boost::system
)

