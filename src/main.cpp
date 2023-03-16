#include <iostream>
#include <string>
#include <vector>

#include "ArgParser.hpp"

int main(int argc, const char** argv) {
  std::cout << "Welcome to includes scanner!" << std::endl;

  if (!ArgParser::parse(argc, argv)) {
    std::cout << "Command line arguments is not valid. Terminating.";
    return -1;
  }

  for (auto const& dir : ArgParser::sourceDirs)
    std::cout << "Source directory: [" << dir << "]" << std::endl;
  for (auto const& dir : ArgParser::includeDirs)
    std::cout << "Include directory: [" << dir << "]" << std::endl;

  return 0;
}