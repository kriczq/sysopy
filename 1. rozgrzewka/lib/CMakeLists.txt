set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

# list of source files
set(libsrc lib.c)

set(lib_static OFF CACHE BOOL "library type")

# this is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${libsrc})

# shared libraries need PIC
set_property(TARGET ${objlib} PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(_shared SHARED $<TARGET_OBJECTS:objlib>)
add_library(_static STATIC $<TARGET_OBJECTS:objlib>)

if (lib_static)
    target_compile_definitions(_shared PUBLIC -DLIB_STATIC)
    target_compile_definitions(_static PUBLIC -DLIB_STATIC)

    ##set_target_properties(_shared PROPERTIES COMPILE_DEFINITIONS "LIB_STATIC")
    ##set_target_properties(_static PROPERTIES COMPILE_DEFINITIONS "LIB_STATIC")
    MESSAGE("statyczna alkocja pamieci")
endif()