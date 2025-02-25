# Homework Assignment #2

The second homework is a written one, with some coding.

1. Pen & paper assignment:  Do the exercises 2.4(b), 2.5(c), 3.3(b,d), 3.4, 3.12(a, b) in the textbook. (Note that the example given in 3.3(d) in the textbook is wrong. It should be “[([])()[(][])]”.)
2. For exercise 2.4(b), 2.5(c), write the lex programs, respectively, that will accept exactly those strings (given in the *.txt files under the test directory) that are described in them.
3. For 3.3(d), write the yacc program that will accept exactly those described strings. The program should test all the input files (*.txt) under the test directory.
4. For 3.12(a), produce the LR(0) parsing table, and then code a parser in C++ (NOT using YACC) using the technique described in Figure 3.18 (in the textbook) that parses an input string in the test files.

## Note1

3.3(d) 说明如下。本题的说明是：Balanced parentheses and brackets, where a closing bracket also closes any outstanding open parentheses (up to the previous open bracket). 理解一下：

1.先考虑什么是Balanced parentheses and brackets，应该指的是一个open parenthesis需要有一个close parenthesis对应，而（这个很重要）在这一对parenthesis/brackets之间，所有parentheses和brackets都已经balance掉了。
2.在本题目的场景下，说是一个close bracket可以去close掉“所有open parentheses（可以0或多个），直到它找到对应的open bracket。所以课本给的例子：“[([](()[(][])]”是对的。我们可以这样看一下：第3-4个符号[]不影响任何东西，所以可以先“去掉”，得出：[((()[(][])]，其中第4-5个符号互相balance掉，得出：[(([(][])]，然后这个里面的第6个符号（第一个close）是个bracket，它可以close掉所有open parentheses，所以我们把第5个符号（是一个open parenthesis）“吃掉”并balance out前面一个open bracket，得出[(([])]，之后又有“平常意义下“balance的brackets和parentheses，得出[(]。最后一个close bracket把open parenthesis“吃掉”并与最早那个bracket平衡掉。所以这个例子是对的。
3.课本给的Hint: First, make the language of balanced parentheses and brackets, where extra open parentheses are allowed; then make sure this nonterminal must appear within brackets. 这个hint说的是可以用一个nonterminal产出任何个open parentheses，只要他们被两个平衡的brackets包含住即可。

## Note2

You should scan your written solutions and put into the report.pdf (under your doc directory), and use "make handin" to wrap everything into a zip file to submit on elearning.

Note that 2.4(b), 2.5(c), 3.3(d) need you to write programs with lex and yacc (note: don't use lex/yacc for 3.12(a)). Do:

* Put all your code under the corresponding directory.
* Change the Makefile under "HW2" that will go into the subdirectories to complete the commands

So that you may use "make test" command under HW2 to execute all your programs to test all the corresponding test cases under all the "test" directories.

## 补充说明

1. 纸面作业：可以借助网站[CFG Developer (stanford.edu)](https://web.stanford.edu/class/archive/cs/cs103/cs103.1156/tools/cfg/)验证语法是否非歧义。`report.pdf`只需包含每题的解答即可。
2. 编程作业：
   1. 每题的实现都位于 `HW2/tools/`下的一个文件夹。例如题目 `question`，位于 `HW2/tools/question`，需要编写相应的C++/lex/yacc等文件，并自行编写 `Makefile`/`CMake`对代码进行编译。要求：`Makefile`中有 `make build`、`make clean`和 `make test`命令：
      - `make build`生成可执行的 `HW2/tools/question/build/main.out`
      - `make clean`删除 `HW2/tools/question/build`文件夹以及所有编译产生的中间文件
      - `make test`使用 `HW2/tools/question/build/main.out`对所有 `HW2/test/question/`文件夹下的 `.txt`文件进行测试，每个 `.txt`文件的输出结果是 `accept`或 `reject`
   2. 每题的测试都位于 `HW2/test/`下的一个文件夹。例如题目 `question`，位于 `HW2/test/question`，每个测试是一个 `.txt`文件。
   3. 对所有题目进行一次性构建和测试：在主文件夹 `HW2`下运行 `make build`、`make clean`和 `make test`命令，会调用所有题目文件夹下的 `make build`、`make clean`和 `make test`命令。请自行修改主文件夹下的 `Makefile`
