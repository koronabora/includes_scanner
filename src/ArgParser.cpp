#include "ArgParser.hpp"

#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

std::vector<std::string> ArgParser::sourceDirs;
std::vector<std::string> ArgParser::includeDirs;

bool ArgParser::parse(int argc, const char** argv) {
  try {
    po::options_description desc("Allowed options");
    desc.add_options()("help", "Help message")(
        "input", po::value<std::vector<std::string>>(), "Input dirs")(
        "include,I", po::value<std::vector<std::string>>(), "Includes");

    // make <input> as parameter
    po::positional_options_description p;
    p.add("input", -1);

    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv).options(desc).positional(p).run(),
        vm);
    po::notify(vm);

    sourceDirs = vm["input"].as<std::vector<std::string>>();
    includeDirs = vm["include"].as<std::vector<std::string>>();
  } catch (po::error const& ex) {
    std::cerr << ex.what() << std::endl;
    return false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    return false;
  } catch (...) {
    return false;
  }
  return true;
}