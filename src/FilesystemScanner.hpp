#ifndef FILESYSTEM_SCANNER_HPP
#define FILESYSTEM_SCANNER_HPP

#include <string>
#include <vector>
#include <future>
#include <mutex>

class FilesystemScanner {
  const std::vector<std::string_view> HEADER_EXTENSIONS{".h", ".hpp"};
  const std::vector<std::string_view> SOURCE_EXTENSIONS{".c", ".cpp"};

 public:
  void scanForHeaders(std::string_view path);
  void scanForSources(std::string_view path);

  void waitUntilResults();

  std::vector<std::string>&& takeSourceFiles();
  std::vector<std::string>&& takeHeaderFiles();

private:
  std::vector<std::string> m_sourceFiles;
  std::vector<std::string> m_headerFiles;

  std::vector<std::string> _scan(std::string_view path,
                                 std::vector<std::string_view> const& targetExt);

  std::vector<std::future<std::vector<std::string>>> m_headerResultHandlers;
  std::vector<std::future<std::vector<std::string>>> m_sourceResultHandlers;
};

#endif  // FILESYSTEM_SCANNER_HPP