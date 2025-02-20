#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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
  std::cout << std::endl;

  file.close();
  return wordCount;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[1];
  int result = parse_file(filename);

  std::cout << "The result is: " << result << std::endl;

  return 0;
}