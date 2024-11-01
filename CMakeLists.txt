#mqtt模块x86编译工程
project("libuv_demo")
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g")

add_executable(hello_uv hello_uv.c)
target_link_libraries(hello_uv
uv
)

add_executable(tcp_server tcp_server.c)
target_link_libraries(tcp_server
uv
)

add_executable(tcp_client tcp_client.c)
target_link_libraries(tcp_client
uv
)