**Homework Assignment 3**

**See more on eLearning.**

This assignment is to

1. parse the full FDMJ into FDMJ-AST, as an extension of the parser from HW1 (paser.yy and lexer.ll are copied from HW1 and you need to change them)，
2. write a visitor of the AST (using the template given in setnamemaps.cc) to produce semantic map for names (namemap), and
3. write another visitor of the AST (using the template given in semantanlyzer.cc) to add semantic information to each AST node (if applicable).

**namemap.hh** gives the relevant classes to collect the names (identifiers) in the program to remember their meaning. For example, an id is a class name, method name, a variable in a class, and a formal in a method of a class, etc. This is useful for doing the second visitor above.

**semant.hh** gives the relevant classes to assign semantic to each AST node (if applicable). We mostly care about AST nodes that represent a value, in which case, we need to know the data type of the value (class obj, int, array) and whether it's an lvalue. An AST node could also be a method name or a class name.

**Semantic check** not only finds out the semantic of each AST node, and also check if the type of a node is correct. Various type checking needs to be done, including for example, the left-hand-side of an assignment is an lvalue (or not), the method call parameters match up with the declaration of the method in terms of type type. Note that your semantic check also needs to check if continue/break is in a while loop. **Your report should give all the type checking your program does. Missing apparant type checking is a cause for deducting points.**

Your program may report the first type-error (if any) and quit. Or if you are brave, try to continue the type-checking to find more errors.

Some more notes below:

* The full FDMJ AST definition is given in the FDMJAST.hh file (with accompanying FDMJAST.cc code). The semantic map definition is given in the namemap.hh and semant.hh files. See description of these three hh files in the docs directory.
* Conversion code between FDMJAST (with/without semantic information) and XML is given. You should use it to print the parsing result using the given code (as exemplified in main.cc). Example AST output may be found under test/output_example directory.
* Use eLearning to **submit** your code package (follow the instructions).

**Your code will be tested with more test programs (fmj files) for grading.**

补充说明：

1. 本作业分为语法解析和类型检查两部分。
   1. 语法解析部分位于`frontend`文件夹。总体是将HW1的FDMJ-SLP parser改为FDMJ parser。基本代码结构不变，只需要需修改lexer.ll及parser.yy，以及相关的header文件（比如ASTLexer.hh）。FDMJAST的定义以及相关XML的代码已给定。
   2. 类型检查部分位于`ast`文件夹。`Name_Maps`数据结构不变，需要实现的是`makeNameMaps`和`semant_analyze`的相关逻辑。
2. 自行修改 `tools/main/main.cc`进行测试，最后测评时我们将使用统一的main。需要暴露的接口如下；
   1. `Program* fdmjParser(const string &, const bool);`
   2. `Program* fdmjParser(ifstream &, const bool);`
   3. `Name_Maps* makeNameMaps(Program* );`
   4. `AST_Semant_Map* semant_analyze(Program* );`
      - 注意到，`makeNameMaps`会在 `semant_analyze`中被调用
3. 接口说明如下
   - 如果fmj语法错误，则 `fdmjParser`会报错（输出错误位置和错误原因），然后程序直接退出
   - 如果fmj语法正确，但类型检查错误，则 `makeNameMaps`或 `semant_analyze`会报错（输出错误位置和错误原因），然后程序直接退出
   - 如果fmj语法正确，且类型检查正确，则程序正常运行完成
