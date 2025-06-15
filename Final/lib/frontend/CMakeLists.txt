# flex和bison
find_package(FLEX 2.6)
find_package(BISON 3.8)
FLEX_TARGET(lexer lexer.ll ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)
BISON_TARGET(parser parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cc 
                        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hh)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

aux_source_directory(. FRONTEND_SRCS)
add_library(frontend OBJECT 
  ${FRONTEND_SRCS}
  ${FLEX_lexer_OUTPUTS}
  ${BISON_parser_OUTPUTS}
)