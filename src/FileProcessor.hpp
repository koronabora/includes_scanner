#ifndef FILE_PROCESSOR_HPP
#define FILE_PROCESSOR_HPP

#include <string_view>
#include <vector>
#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, size_t> fmap;

class FileProcessor {
 public:
  void processFile(std::string_view file, std::vector<std::string>& includeDirs);
  fmap&& takeResults();
 private:
  fmap m_fileData;
};

#endif  // FILE_PROCESSOR_HPP