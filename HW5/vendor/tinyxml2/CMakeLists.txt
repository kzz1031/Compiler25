set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result --std=c++17")
aux_source_directory(. VENDOR_LXML_SRCS)
add_library(vendor_xml OBJECT ${VENDOR_LXML_SRCS})