cmake_minimum_required (VERSION 3.0)
project (HELLO)

enable_testing()

#ADD_CUSTOM_TARGET(debug
#  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
#  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
#  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
#  )


#set(LIBRARY_OUTPUT_PATH lib)
add_subdirectory(lib)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")



include_directories(inc)

file(GLOB SOURCES "src/*.c")

add_executable(main ${SOURCES})
target_link_libraries(main lib)
set_property(TARGET main PROPERTY C_STANDARD 99) #??


# fixing cache problems


#add_test(NAME test_fdsfds_${i} COMMAND sh -c "cmake -Dlib_type=${i} -Dlib_static=OFF .. && make && ${run_command} >> ../test/resultstest.txt")

############ testy
set(run_command "./main")
set(test_output "../test/wyniki.txt")
set(test_srcfile "../res/a.txt")
set(test_destfile "../res/b.txt")
set(record_num 1000)


foreach(record_size 4 512 4096 8192)
    add_test(NAME test_generate_${record_size} COMMAND sh -c "${run_command} --generate --src ${test_srcfile} --num ${record_num} --record ${record_size}")
    
    add_test(NAME test_copy_${record_size} COMMAND sh -c "${run_command} --copy --src ${test_srcfile} --dest ${test_destfile} --num ${record_num} --record ${record_size} >> ${test_output}")
    add_test(NAME test_copy_lib_${record_size} COMMAND sh -c "${run_command} --copy --src ${test_srcfile} --dest ${test_destfile} --num ${record_num} --record ${record_size} --lib >> ${test_output}")
    
    add_test(NAME test_sort_${record_size} COMMAND sh -c "${run_command} --sort --src ${test_srcfile} --num ${record_num} --record ${record_size} >> ${test_output}")
    add_test(NAME test_sort_lib_${record_size} COMMAND sh -c "${run_command} --sort --src ${test_destfile} --num ${record_num} --record ${record_size} --lib >> ${test_output}")
endforeach()

