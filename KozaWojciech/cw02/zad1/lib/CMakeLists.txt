set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

# list of source files
set(libsrc lib.c)


# this is the "object library" target: compiles the sources only once
#add_library(objlib OBJECT ${libsrc})

# shared libraries need PIC
#set_property(TARGET ${objlib} PROPERTY POSITION_INDEPENDENT_CODE 1)
#set_property(TARGET ${objlib} PROPERTY C_STANDARD 99) #??

# shared and static libraries built from the same object files
add_library(lib SHARED ${libsrc})
#add_library(lib STATIC ${libsrc})
set_property(TARGET lib PROPERTY C_STANDARD 99) #??