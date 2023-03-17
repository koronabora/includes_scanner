#include "FilesystemScanner.hpp"

#include <filesystem>
#include <iostream>
#include <iterator>

namespace fs = std::filesystem;

std::vector<std::string> FilesystemScanner::_scan(
    std::string_view path, std::vector<std::string_view> const& targetExts) {
  fs::path const rootPath{path};
  fs::recursive_directory_iterator begin(rootPath);
  fs::recursive_directory_iterator end;

  std::vector<fs::path> files;
  std::copy_if(begin, end, std::back_inserter(files),
               [&targetExts](fs::path const& p) {
                 return fs::is_regular_file(p) &&
                        (std::find(targetExts.cbegin(), targetExts.cend(),
                                   p.extension()) != targetExts.end());
               });

  std::vector<std::string> out;
  std::transform(files.cbegin(), files.cend(), std::back_inserter(out),
                 [&rootPath](fs::path const& p) {
                   return fs::relative(p, rootPath).string();
                 });
  return out;
}

void FilesystemScanner::scanForHeaders(std::string_view path) {
  std::cout << "Scanning folder [" << path << "] for the header files"
            << std::endl;
  auto futureHandler = std::async(std::launch::async, &FilesystemScanner::_scan,
                                  this, path, std::ref(HEADER_EXTENSIONS));

  m_headerResultHandlers.push_back(std::move(futureHandler));
}

void FilesystemScanner::scanForSources(std::string_view path) {
  std::cout << "Scanning folder [" << path << "] for the source files"
            << std::endl;
  auto futureHandler = std::async(std::launch::async, &FilesystemScanner::_scan,
                                  this, path, std::ref(SOURCE_EXTENSIONS));
  m_sourceResultHandlers.push_back(std::move(futureHandler));
}

void FilesystemScanner::waitUntilResults() {
  for (auto& f : m_headerResultHandlers) {
    auto results = f.get();
    m_headerFiles.insert(m_headerFiles.end(),
                         std::make_move_iterator(results.begin()),
                         std::make_move_iterator(results.end()));
  }
  m_headerResultHandlers.clear();
  for (auto& f : m_sourceResultHandlers) {
    auto results = f.get();
    m_sourceFiles.insert(m_sourceFiles.end(),
                         std::make_move_iterator(results.begin()),
                         std::make_move_iterator(results.end()));
  }
  m_sourceResultHandlers.clear();
  std::cout << "Scan complete. Found "
            << std::to_string(m_headerFiles.size()) << " header files and " << std::to_string(m_sourceFiles.size()) 
            << " source files"
            << std::endl;
}

std::vector<std::string>&& FilesystemScanner::takeSourceFiles() {
  return std::move(m_sourceFiles);
}
std::vector<std::string>&& FilesystemScanner::takeHeaderFiles() {
  return std::move(m_headerFiles);
}