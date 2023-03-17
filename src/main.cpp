#include <iostream>
#include <string>
#include <vector>

#include "ArgParser.hpp"
#include "FileProcessor.hpp"
#include "FilesystemScanner.hpp"

static const sview_vector HEADER_EXTENSIONS{".h", ".hpp"};
static const sview_vector SOURCE_EXTENSIONS{".c", ".cpp"};

int main(int argc, const char** argv) {
  std::cout << "Welcome to includes scanner!" << std::endl;

  // Get input
  //*************************************************************
  std::cout << "------------------------------" << std::endl;
  if (!ArgParser::parse(argc, argv)) {
    std::cout << "Command line arguments are not valid. Terminating." << std::endl;
    return -1;
  }

  for (auto const& dir : ArgParser::sourceDirs)
    std::cout << "Source directory: [" << dir << "]" << std::endl;
  for (auto const& dir : ArgParser::includeDirs)
    std::cout << "Include directory: [" << dir << "]" << std::endl;

  // Scan filesystem
  //*************************************************************
  std::cout << "------------------------------" << std::endl;
  std::cout << "Starting scanning directories" << std::endl;

  FilesystemScanner fscan;
  // scan for sources in source directories
  for (auto const& dir : ArgParser::sourceDirs)
    fscan.scanForFiles(dir, SOURCE_EXTENSIONS, false);
  fscan.waitUntilResults();
  svector sources{fscan.takeFiles()};
  std::cout << "Found " << std::to_string(sources.size()) << " source files"
            << std::endl;

  // Read files and parse
  //*************************************************************
  
  if (!sources.empty()) {
    std::cout << "------------------------------" << std::endl;
    std::cout << "Processing source files" << std::endl;
    FileProcessor fp;
    for (auto& f : sources) fp.processFile(f, ArgParser::includeDirs);

    fp.waitUntilResults();

    auto results{fp.takeResults()};
    fmap stats;

    std::cout << "Finished" << std::endl;

    for (auto const& p : results) {
      std::cout << "------------------------------" << std::endl;
      std::cout << p.fileName << std::endl;
      for (auto const& vals : p.includes) {
          // print to console
        for (std::size_t i = 0; i < vals.second; ++i) std::cout << ". ";
        std::cout << vals.first << std::endl;
        // save to stats
        ++stats[vals.first];
      }
    }

    std::cout << "------------------------------" << std::endl;
    std::cout << "Total:" << std::endl;
    for (auto const& p : stats) {
      std::cout << p.first << ": " << std::to_string(p.second) << std::endl;
    }
  }
  std::cout << "------------------------------" << std::endl;
  std::cout << "Program finished! Have a nice day!";

  return 0;
}