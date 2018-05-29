cmake_minimum_required (VERSION 3.0)
project (HELLO)

enable_testing()

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

add_executable(main ${SOURCES})
set_property(TARGET main PROPERTY C_STANDARD 99) #??
target_link_libraries(main pthread)

add_test(NAME test COMMAND bash -c
"for FILTER in {03,10,65}; do                                       \
    for THREAD in {1,2,4,8}; do                         \
        ./main $$THREAD                             \
        ../image_in.pgm                                \
        ../filters/filter$$FILTER                      \
        ../test/out.$$THREAD.$$FILTER.pgm >> ../test/Times.txt; \
    done;                                               \
done")