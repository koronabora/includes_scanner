#include <iostream>
#include <string>
#include <vector>

#include "ArgParser.hpp"
#include "FilesystemScanner.hpp"
#include "FileProcessor.hpp"

static const sview_vector HEADER_EXTENSIONS{".h", ".hpp"};
static const sview_vector SOURCE_EXTENSIONS{".c", ".cpp"};

svector&& makeScan(FilesystemScanner& fscan, svector const& dirs,
                   sview_vector const& extensions, bool const makeRelative = true) {
  for (auto const& dir : dirs) fscan.scanForFiles(dir, extensions, makeRelative);
  fscan.waitUntilResults();
  return std::move(fscan.takeFiles());
}

int main(int argc, const char** argv) {
  std::cout << "Welcome to includes scanner!" << std::endl;

  // Get input
  //*************************************************************
  if (!ArgParser::parse(argc, argv)) {
    std::cout << "Command line arguments are not valid. Terminating.";
    return -1;
  }

  for (auto const& dir : ArgParser::sourceDirs)
    std::cout << "Source directory: [" << dir << "]" << std::endl;
  for (auto const& dir : ArgParser::includeDirs)
    std::cout << "Include directory: [" << dir << "]" << std::endl;

  // Scan filesystem
  //*************************************************************
  std::cout << "Start scanning directories" << std::endl;

  FilesystemScanner fscan;  // some ugly copypasta below

  // scan for sources in source directories
  svector sources{makeScan(fscan, ArgParser::sourceDirs, SOURCE_EXTENSIONS, false)};
  std::cout << "Found " << std::to_string(sources.size()) << " source files"
            << std::endl;

  // scan for headers in source directories
  svector localHeaders{
      makeScan(fscan, ArgParser::sourceDirs, HEADER_EXTENSIONS)};
  std::cout << "Found " << std::to_string(localHeaders.size())
            << " local header files" << std::endl;

  // scan for headers in include directories
  svector headers{makeScan(fscan, ArgParser::includeDirs, HEADER_EXTENSIONS)};
  std::cout << "Found " << std::to_string(headers.size()) << " header files"
            << std::endl;

  // Read files and parse
  //*************************************************************
  std::cout << "Start sources preprocessing" << std::endl;

  if (!sources.empty()) {
    FileProcessor fp;
    for (auto& f: sources)
      fp.processFile(f, ArgParser::includeDirs);
  }

  return 0;
}