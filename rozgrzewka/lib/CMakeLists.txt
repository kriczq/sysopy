set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

# list of source files
set(libsrc lib.c)

# this is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${libsrc})

# shared libraries need PIC
set_property(TARGET ${objlib} PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(_shared SHARED $<TARGET_OBJECTS:objlib>)
add_library(_static STATIC $<TARGET_OBJECTS:objlib>)