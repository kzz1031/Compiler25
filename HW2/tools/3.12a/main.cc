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
    std::string content;
    
    // 读取整个文件内容
    std::getline(file, content, '\0');
    
    size_t pos = 0;
    while (pos < content.length()) {
        // 跳过空白字符
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        // 处理标识符 "id"
        if (content.substr(pos, 2) == "id") {
            tokens.push_back("id");
            pos += 2;
            continue;
        }
        
        // 处理特殊字符
        if (content[pos] == '(' || content[pos] == ')' || content[pos] == '+') {
            tokens.push_back(std::string(1, content[pos]));
            pos++;
            continue;
        }
    }
    
    tokens.push_back("$"); // 添加结束标记
    return tokens;
}

// 定义LR(0)解析表
std::map<std::pair<int, std::string>, std::string> parsing_table = {
    {{0, "id"}, "s2"}, {{0, "E"}, "s1"},
    {{1, "+"}, "s3"}, {{1, "$"}, "acc"},
    {{2, "+"}, "r1"}, {{2, ")"}, "r1"}, {{2, "$"}, "r1"}, {{2, "id"}, "r1"}, {{2, "("}, "s4"},
    {{3, "id"}, "s5"},
    {{4, "id"}, "s2"}, {{4, "E"}, "s6"},
    {{5, "+"}, "r3"}, {{5, ")"}, "r3"}, {{5, "$"}, "r3"}, {{5, "id"}, "r3"}, {{5, "("}, "r3"},
    {{6, "+"}, "s3"}, {{6, ")"}, "s7"},
    {{7, "+"}, "r2"}, {{7, ")"}, "r2"}, {{7, "$"}, "r2"}, {{7, "id"}, "r2"}, {{7, "("}, "r2"}
};

std::map<int, std::pair<std::string, int>> productions = {
    {0, {"E", 1}}, {1, {"E", 1}}, {2, {"E", 4}}, {3, {"E", 3}}
};

bool parse(const std::vector<std::string> &tokens) {
    std::stack<int> states;
    states.push(0);
    int index = 0;

    while (index < int(tokens.size())) {
        int state = states.top();
        std::string token = tokens[index];
        auto action = parsing_table.find({state, token});
        //std::cout << "State: " << state << ", Token: " << token << std::endl;
        if (action == parsing_table.end()) {
            std::cerr << "Syntax error at token " << token << std::endl;
            return false;
        }
        //std::cout<<"action: "<<action->second<<std::endl;
        if (action->second[0] == 's') {
            states.push(std::stoi(action->second.substr(1)));
            ++index;
        } else if (action->second[0] == 'r') {
            int prod_num = std::stoi(action->second.substr(1));
            auto prod = productions[prod_num];
            for (int i = 0; i < prod.second; ++i) {
                //std::cout << "  pop " << states.top() << std::endl;
                if(states.size() == 0) {
                    std::cerr << "Error" << token << std::endl;
                    return false;
                }
                states.pop();
            }
            
            auto goto_action = parsing_table.find({states.top(), prod.first});
            if (goto_action == parsing_table.end()) {
                std::cerr << "Goto error at state " << states.top() << " with production " << prod.first << std::endl;
                return false;
            }
            states.push(goto_action->second[1] - '0');
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
    std::cout << "Tokens: ";
    for (const auto& token : tokens) {
        std::cout << token << " ";
    }
    std::cout << std::endl;
    
    if (parse(tokens)) {
        std::cout << "accept" << std::endl;
    } else {
        std::cout << "reject" << std::endl;
    }

    return 0;
}