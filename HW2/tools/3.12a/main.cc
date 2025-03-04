#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stack>

int parse_file(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Could not open the file!" << std::endl;
    return -1;
  }
  std::string word;
  int wordCount = 0;
  while (file >> word) {
    std::cout << word << " ";
    ++wordCount;
  }
  std::cout << "word count: " <<wordCount;
  std::cout << std::endl;

  file.close();
  return wordCount;
}

std::vector<std::string> tokenize(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<std::string> tokens;
    std::string token;
    while (file >> token) {
        tokens.push_back(token);
    }
    tokens.push_back("$"); // End of input marker
    return tokens;
}

// 定义LR(0)解析表
std::map<std::pair<int, std::string>, std::string> parsing_table = {
    {{0, "id"}, "s1"}, {{0, "E"}, "2"},
    {{1, "("}, "s3"}, {{1, "+"}, "r1"}, {{1, ")"}, "r1"}, {{1, "$"}, "r1"},
    {{2, "+"}, "s4"}, {{2, ")"}, "r2"}, {{2, "$"}, "acc"},
    {{3, "id"}, "s1"}, {{3, "E"}, "5"},
    {{4, "id"}, "s1"}, {{4, "E"}, "6"},
    {{5, ")"}, "s7"},
    {{6, "+"}, "r3"}, {{6, ")"}, "r3"}, {{6, "$"}, "r3"},
    {{7, "+"}, "r2"}, {{7, ")"}, "r2"}, {{7, "$"}, "r2"}
};

std::map<int, std::pair<std::string, int>> productions = {
    {1, {"E", 1}}, {2, {"E", 3}}, {3, {"E", 3}}
};

bool parse(const std::vector<std::string> &tokens) {
    std::stack<int> states;
    states.push(0);
    int index = 0;

    while (index < int(tokens.size())) {
        int state = states.top();
        std::string token = tokens[index];
        auto action = parsing_table.find({state, token});

        if (action == parsing_table.end()) {
            std::cerr << "Syntax error at token " << token << std::endl;
            return false;
        }

        if (action->second[0] == 's') {
            states.push(std::stoi(action->second.substr(1)));
            ++index;
        } else if (action->second[0] == 'r') {
            int prod_num = std::stoi(action->second.substr(1));
            auto prod = productions[prod_num];
            for (int i = 0; i < prod.second; ++i) {
                states.pop();
            }
            states.push(parsing_table[{states.top(), prod.first}][0] - '0');
        } else if (action->second == "acc") {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int result = parse_file(filename);

    std::cout << "The result is: " << result << std::endl;

    std::vector<std::string> tokens = tokenize(filename);
    if (parse(tokens)) {
        std::cout << "Parsing successful!" << std::endl;
    } else {
        std::cout << "Parsing failed!" << std::endl;
    }

    return 0;
}