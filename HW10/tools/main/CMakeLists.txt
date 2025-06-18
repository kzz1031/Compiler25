# 指定生成目标
set(MAIN "main")
add_executable(${MAIN} main.cc)
target_compile_features(${MAIN} PUBLIC cxx_std_17)

# 添加链接库
target_link_libraries(${MAIN} PUBLIC
  util
  quad
  ir
  opt
  vendor_xml
)
