#include <iostream>
#include <string>
#include <vector>

#include "ArgParser.hpp"
#include "FilesystemScanner.hpp"

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

  FilesystemScanner fscan;
  for (auto const& dir : ArgParser::sourceDirs) fscan.scanForSources(dir);
  for (auto const& dir : ArgParser::includeDirs) fscan.scanForHeaders(dir);
  fscan.waitUntilResults();

  std::vector<std::string> sources{fscan.takeSourceFiles()};
  std::vector<std::string> headers{fscan.takeHeaderFiles()};

  // Read files and parse
  //*************************************************************

  return 0;
}