cmake_minimum_required (VERSION 3.0)
project (HELLO)

add_executable(server "server.c" "additions.h")
set_property(TARGET server PROPERTY C_STANDARD 99) #??
target_link_libraries(server pthread)

add_executable(client "client.c" "additions.h")
set_property(TARGET client PROPERTY C_STANDARD 99) #??
target_link_libraries(client pthread)