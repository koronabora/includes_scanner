#include "FilesystemScanner.hpp"

#include <filesystem>
#include <iostream>
#include <iterator>

namespace fs = std::filesystem;

svector FilesystemScanner::_scan(std::string_view path,
                                 sview_vector const& targetExts,
                                 bool const makeRelative) {
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

  svector out;
  std::transform(files.cbegin(), files.cend(), std::back_inserter(out),
                 [&rootPath, &makeRelative](fs::path const& p) {
                   return makeRelative ? fs::relative(p, rootPath).string() : p.string();
                 });
  return out;
}

void FilesystemScanner::scanForFiles(std::string_view path,
                                     sview_vector const& extensions,
                                     bool const makeRelative) {
  try {
    auto futureHandler =
        std::async(std::launch::async, &FilesystemScanner::_scan, this, path,
                   std::ref(extensions), makeRelative);

    m_resultHandlers.push_back(std::move(futureHandler));
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
  }
}

void FilesystemScanner::waitUntilResults() {
  try {
    for (auto& f : m_resultHandlers) {
      auto results = f.get();
      m_files.insert(m_files.end(), std::make_move_iterator(results.begin()),
                     std::make_move_iterator(results.end()));
    }
    m_resultHandlers.clear();
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
  }
}

svector&& FilesystemScanner::takeFiles() { return std::move(m_files); }