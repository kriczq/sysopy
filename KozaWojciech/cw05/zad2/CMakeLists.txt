cmake_minimum_required (VERSION 3.0)
project (HELLO)

#enable_testing()

#ADD_CUSTOM_TARGET(debug
#  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
#  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
#  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
#  )


#set(LIBRARY_OUTPUT_PATH lib)
#add_subdirectory(lib)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")



#include_directories(inc)

file(GLOB SOURCES "src/*.c")

add_executable(master "src/master.c")
set_property(TARGET master PROPERTY C_STANDARD 99) #??

add_executable(slave "src/slave.c")
set_property(TARGET slave PROPERTY C_STANDARD 99) #??