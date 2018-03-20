cmake_minimum_required (VERSION 3.0)
project (HELLO)

set(LIB_TYPE "static" CACHE STRING "library type")
enable_testing()

#ADD_CUSTOM_TARGET(debug
#  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
#  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
#  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
#  )

#option(SHARED "Build the shared library" OFF)

#set(LIBRARY_OUTPUT_PATH lib)
add_subdirectory(lib)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(run_command "./main 10000 100 -a 1000 -r 898 -f fdsafadsfdsfsa")



include_directories(inc)

file(GLOB SOURCES "src/*.c")

add_executable(main ${SOURCES})
set_property(TARGET main PROPERTY C_STANDARD 99) #??


if (LIB_TYPE STREQUAL "dynamic")
    MESSAGE("dynamiczna kompilowanie")
    #target_compile_definitions(main PRIVATE -DDYNAMIC)
    set_target_properties(main PROPERTIES COMPILE_DEFINITIONS "DYNAMIC")
elseif(LIB_TYPE STREQUAL "shared")
    target_link_libraries(main _shared)
else()
    target_link_libraries(main _static)
endif()

if (LIB_STATIC)
    #target_compile_definitions(main PUBLIC -DLIB_STATIC)
    set_target_properties(main PROPERTIES COMPILE_DEFINITIONS "LIB_STATIC")
    ##set_target_properties(_shared PROPERTIES COMPILE_DEFINITIONS "LIB_STATIC")
    ##set_target_properties(_static PROPERTIES COMPILE_DEFINITIONS "LIB_STATIC")
    MESSAGE("statyczna alkocja pamieci")
endif()
