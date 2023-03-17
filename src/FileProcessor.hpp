#ifndef FILE_PROCESSOR_HPP
#define FILE_PROCESSOR_HPP

#include <string_view>
#include <vector>
#include <string>

class FileProcessor {
 public:
  void processFile(std::string_view file, std::vector<std::string>& includeDirs);
};

#endif  // FILE_PROCESSOR_HPP