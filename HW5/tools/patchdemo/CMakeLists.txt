set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result --std=c++17")


# 指定生成目标
set(PATCHDEMO "patchdemo")
add_executable(${PATCHDEMO} patchdemo.cc)
target_compile_features(${PATCHDEMO} PUBLIC cxx_std_17)

# 添加链接库
target_link_libraries(${PATCHDEMO} PUBLIC
  util
  ir
  ast
  vendor_xml
)
