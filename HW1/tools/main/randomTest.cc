#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "ast2xml.hh"
#include "executor.hh"
#include "tinyxml2.hh"

using namespace fdmj;
using namespace tinyxml2;

using pii = std::pair<int, int>;
using psi = std::pair<std::string, int>;

psi randomBlock(std::vector<std::string> &variables);

std::map<std::string, int> varMap;

// 随机生成器
int getRandomInt(int min, int max) {
  return min + std::rand() % (max - min + 1);
}

// 随机生成变量名
std::string randomVariable() {
  char var = 'a' + getRandomInt(0, 25); // a-z
  return std::string(1, var);
}

// 随机生成整数
psi randomInteger() {
  int a = getRandomInt(-100, 100);
  if (a < 0) {
    return {"(" + std::to_string(a) + ")", a};
  }
  return {std::to_string(a), a};
}

// 随机生成基本表达式
psi randomExpression(std::vector<std::string> &variables) {
  int choice = getRandomInt(0, 1);
  if (choice == 0 && !variables.empty()) {
    // 返回一个随机变量
    int idx = getRandomInt(0, variables.size() - 1);
    auto var = variables[idx];
    return {var, varMap[var]};
  } else {
    // 返回一个整数
    return randomInteger();
  }
}

// 随机生成算术表达式
psi randomArithmeticExpression(std::vector<std::string> &variables,
                               bool force = false) {
  int choice = getRandomInt(0, 1);
  psi expr1 = force || choice == 1 ? randomExpression(variables)
                                   : randomBlock(variables);
  choice = getRandomInt(0, 2);
  psi expr2 = force || choice == 1 ? randomExpression(variables)
                                   : randomBlock(variables);
  switch (getRandomInt(0, 3)) {
  case 0:
    return {"(" + expr1.first + "+" + expr2.first + ")",
            expr1.second + expr2.second};
  case 1:
    return {"(" + expr1.first + "-" + expr2.first + ")",
            expr1.second - expr2.second};
  case 2:
    return {"(" + expr1.first + "*" + expr2.first + ")",
            expr1.second * expr2.second};
  default:
  case 3:
    if (expr2.second == 0) {
      throw std::runtime_error("Division by zero");
    }
    return {"(" + expr1.first + "/" + expr2.first + ")",
            expr1.second / expr2.second};
  }; // 随机选择运算符
}

// 随机生成赋值语句
std::string randomAssignment(std::vector<std::string> &variables,
                             bool force = false) {
  std::string var = randomVariable();
  int choice = getRandomInt(0, 1);
  psi expr = force || choice == 1 ? randomArithmeticExpression(variables, force)
                                  : randomBlock(variables);
  variables.push_back(var); // 新变量加入变量列表
  varMap[var] = expr.second;
  return var + "=" + expr.first + ";";
}

// 随机生成花括号代码块
psi randomBlock(std::vector<std::string> &variables) {
  int numStatements = getRandomInt(1, 4);
  std::string block;
  block += "({";
  for (int i = 0; i < numStatements; ++i) {
    block += randomAssignment(variables, true);
  }

  block += "}";
  auto expr = randomArithmeticExpression(variables, true);
  block += expr.first + ")";
  return {block, expr.second};
}

// 随机生成 return 语句
psi randomReturn(std::vector<std::string> &variables) {
  int choice = getRandomInt(0, 2);
  std::string expr;
  int returnVal;
  if (choice == 0) {
    auto res = randomArithmeticExpression(variables);
    expr = res.first;
    returnVal = res.second;
  } else if (choice == 1) {
    auto res = randomBlock(variables);
    expr = res.first;
    returnVal = res.second;
  } else {
    auto res = randomExpression(variables);
    expr = res.first;
    returnVal = res.second;
  }
  expr = "return " + expr + ";";
  return {expr, returnVal};
}

// 随机生成完整的程序
psi generateRandomProgram() {
  std::vector<std::string> variables;
  std::string program = "public int main() {\n";

  int numStatements = getRandomInt(10, 20); // 生成 3~6 行代码
  for (int i = 0; i < numStatements - 1; ++i) {
    program += randomAssignment(variables, false) + "\n";
  }
  auto ret = randomReturn(variables);
  program += ret.first + "\n"; // 最后一行为 return 语句
  program += "}\n";
  return {program, ret.second};
}

int main(int argc, const char *argv[]) {
  feclearexcept(FE_ALL_EXCEPT);
  feenableexcept(FE_ALL_EXCEPT);

  const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;
  std::srand(std::time(NULL)); // 初始化随机种子

  // 生成随机程序
  psi program;

gen:
  try {
    program = generateRandomProgram();
  } catch (const std::exception &e) {
    goto gen;
  }

  // 将程序写入文件
  std::ofstream outFile("random.fmj");
  if (!outFile) {
    std::cerr << "Failed to open file for writing.\n";
    return 1;
  }
  outFile << program.first;
  outFile.close();

  std::ifstream fmjfile("random.fmj");
  std::unique_ptr<Program> root(fdmjParser(fmjfile, debug));
  if (root == nullptr) {
    std::cout << "AST is not valid!" << std::endl;
    return EXIT_FAILURE;
  }

  std::unique_ptr<XMLDocument> x(ast2xml(root.get(), false));
  if (x->Error()) {
    std::cout << "AST is not valid!" << std::endl;
    return EXIT_FAILURE;
  }

  x->SaveFile("random.ast");

  int result = execute(root.get());
  if (result != program.second) {
    std::cout << "Program output is incorrect: expected " << program.second
              << ", got " << result << std::endl;
    return 1;
  } else {
    std::cout << "Program output is correct: " << result << std::endl;
  }
  return 0;
}