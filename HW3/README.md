**Homework Assignment 3**

**See more on eLearning.**

This assignment is to parse the full FDMJ into FDMJ-AST, as an extension of the parser from HW1. 

1. The full FDMJ AST definition is given in the FDMJAST.hh file (with accompanying FDMJAST.cc code). 
2. Conversion code between FDMJAST and XML is given. You should use it to print the parsing result using the given code (as done similarly in main.cc in HW1).
3. Use eLearning to **submit** your code package (follow the instructions). 

**Your code will be tested with more test programs (fmj files) for grading.**

补充说明：

1. 本作业就是在HW1的FDMJ-SLP parser改为FDMJ parser，因此基本代码结构不变。只需要需修改lexer.ll及parser.yy，以及相关的header文件（比如ASTLexer.hh）。FDMJAST的定义以及相关XML的代码已给定。
2. 自行修改`tools/main/main.cc`进行测试。最后测评时我们将使用统一的main (与HW1的main基本一致，只是去掉对任何converter和executor的调用）。
