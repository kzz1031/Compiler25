set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result --std=c++17")


# 指定生成目标
set(MAIN "main")
add_executable(${MAIN} main.cc)
target_compile_features(${MAIN} PUBLIC cxx_std_17)

set(RANDOM_TEST "randomTest")
add_executable(${RANDOM_TEST} randomTest.cc)
target_compile_features(${RANDOM_TEST} PUBLIC cxx_std_17)

# 添加链接库
target_link_libraries(${MAIN} PUBLIC
  frontend
  ast
  vendor_xml
)

target_link_libraries(${RANDOM_TEST} PUBLIC
  frontend
  ast
  vendor_xml
)
