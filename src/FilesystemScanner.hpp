#ifndef FILESYSTEM_SCANNER_HPP
#define FILESYSTEM_SCANNER_HPP

#include <string>
#include <vector>
#include <future>
#include <mutex>

typedef std::vector<std::string> svector;
typedef std::vector<std::string_view> sview_vector;

class FilesystemScanner {
 public:
  void scanForFiles(std::string_view path, sview_vector const& targetExt, bool const makeRelative = true);
  void waitUntilResults();
  svector&& takeFiles();

private:
  svector m_files;
 svector _scan(std::string_view path, sview_vector const& targetExt,
               bool const makeRelative);
  std::vector<std::future<svector>> m_resultHandlers;
};

#endif  // FILESYSTEM_SCANNER_HPP