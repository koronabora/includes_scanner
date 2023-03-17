#ifndef FILE_PROCESSOR_HPP
#define FILE_PROCESSOR_HPP

#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <future>

typedef std::map<std::string, size_t> fmap;
typedef std::pair<std::string, size_t> pairs;

struct FileData {
  std::string fileName;
  std::vector<pairs> includes;  // pairs of values: <include_file, depth>
};

class FileProcessor {
 public:
  void processFile(std::string_view file,
                   std::vector<std::string>& includeDirs);
  void waitUntilResults();
  std::vector<FileData>&& takeResults();

 private:
  std::vector<FileData> m_fileData;
  std::vector<std::future<FileData>> m_resultHandlers;

  FileData _worker(std::string_view file,
                   std::vector<std::string>& includeDirs);
};

#endif  // FILE_PROCESSOR_HPP