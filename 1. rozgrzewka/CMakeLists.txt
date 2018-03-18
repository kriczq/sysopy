cmake_minimum_required (VERSION 3.0)
project (HELLO)

#ADD_CUSTOM_TARGET(debug
#  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
#  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
#  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
#  )

#option(SHARED "Build the shared library" OFF)

#set(LIBRARY_OUTPUT_PATH lib)

set(lib_type "static" CACHE STRING "library type")

#ADD_CUSTOM_TARGET(mylib)

add_subdirectory(lib)

include_directories(inc)

file(GLOB SOURCES "src/*.c")

add_executable(main ${SOURCES})

if (lib_type STREQUAL "dynamic")
    target_compile_definitions(main PUBLIC -DDYNAMIC)
    #set_target_properties(main PROPERTIES COMPILE_DEFINITIONS "DYNAMIC")
elseif(lib_type STREQUAL "shared")
    target_link_libraries(main _shared)
else()
    target_link_libraries(main _static)
endif()

if (lib_static)
    target_compile_definitions(main PUBLIC -DLIB_STATIC)

    ##set_target_properties(_shared PROPERTIES COMPILE_DEFINITIONS "LIB_STATIC")
    ##set_target_properties(_static PROPERTIES COMPILE_DEFINITIONS "LIB_STATIC")
    MESSAGE("statyczna alkocja pamieci")
endif()