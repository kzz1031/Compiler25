set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-result --std=c++17")
aux_source_directory(. IR_SRCS)
add_library(ir OBJECT ${IR_SRCS})