cmake_minimum_required (VERSION 3.0)
project (HELLO)

#ADD_CUSTOM_TARGET(debug
#  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
#  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
#  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
#  )

#option(SHARED "Build the shared library" OFF)

#set(LIBRARY_OUTPUT_PATH lib)

set(lib_type dynamic)

ADD_CUSTOM_TARGET(mylib)

add_subdirectory(lib)

include_directories(inc)

file(GLOB SOURCES "src/*.c")

if (lib_type STREQUAL dynamic)
add_executable(main EXCLUDE_FROM_ALL ${SOURCES})
set_target_properties(main PROPERTIES OUTPUT_NAME "main")
set_target_properties(main PROPERTIES COMPILE_DEFINITIONS "DYNAMIC")
endif()

#static
add_executable(static ${SOURCES})
set_target_properties(static PROPERTIES OUTPUT_NAME "main")

target_link_libraries(static _static)

#shared
add_executable(shared  ${SOURCES})
set_target_properties(shared PROPERTIES OUTPUT_NAME "main")
target_link_libraries(shared _shared)

#dynamic
add_executable(dynamic EXCLUDE_FROM_ALL ${SOURCES})
set_target_properties(dynamic PROPERTIES OUTPUT_NAME "main")
set_target_properties(dynamic PROPERTIES COMPILE_DEFINITIONS "DYNAMIC")