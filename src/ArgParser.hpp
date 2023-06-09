#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <string_view>
#include <vector>

struct ArgParser {
  static bool parse(int argc, const char** argv);
  static std::vector<std::string> sourceDirs;
  static std::vector<std::string> includeDirs;
};

#endif  // ARGPARSER_HPP