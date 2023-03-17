#include "FileProcessor.hpp"

#include <fstream>
#include <iostream>
#include <set>
#include <string_view>
#include <typeinfo>

#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>
#include <boost/wave/preprocessing_hooks.hpp>
#include <boost/wave/cpp_exceptions.hpp>
// #include "lexertl_iterator.hpp"

// Some hints:
// https://github.com/boostorg/wave/blob/develop/samples/quick_start/quick_start.cpp
// https://stackoverflow.com/questions/9449887/removing-c-c-style-comments-using-boostregex
// https://github.com/boostorg/wave/tree/develop/samples/list_includes

typedef std::set<std::string> sset;

///////////////////////////////////////////////////////////////////////////////
//  policy class
struct trace_include_files
    : public boost::wave::context_policies::default_preprocessing_hooks {
  trace_include_files(sset& files_, sset& badFiles_)
      : files(files_), badFiles(badFiles_), include_depth(0) {}

  /* template <typename CtxType>
  bool found_include_directive(CtxType const& ctx, std::string const& filename,
      bool include_next) {
    fileNameBuf = filename;
    //return ::found_include_directive<CtxType>(ctx, filename, include_next);
    return true;
  }*/

  template <typename CtxType>
  void opened_include_file(CtxType const& ctx, std::string const& relname,
                           std::string const& filename,
                           bool is_system_include) {
    auto it = files.find(filename);
    if (it == files.end()) {
      // print indented filename
      for (std::size_t i = 0; i < include_depth; ++i) std::cout << " ";
      std::cout << filename << std::endl;

      files.insert(filename);
    }
    ++include_depth;
  }

  template <typename CtxType>
  void returning_from_include_file(CtxType const& ctx) {
    --include_depth;
  }

  template <typename CtxType, typename ExceptionType>
  void throw_exception(CtxType const& ctx, ExceptionType const& e) {
      // skip bad include file
    if (typeid(ExceptionType) ==
            typeid(boost::wave::preprocess_exception) &&
        e.get_errorcode() ==
        boost::wave::preprocess_exception::error_code::bad_include_file) {

      std::cout << "Bad include file [" << e.description() << "]"
                << std::endl;
      badFiles.insert(e.description());
    } else {
      boost::throw_exception(e);
    }
  }

  /* template <typename ContextT>
  bool found_include_directive(ContextT const& ctx, std::string const& filename,
                               bool include_next);

  template <typename ContextT>
  bool locate_include_file(ContextT& ctx, std::string& file_path,
                           bool is_system, char const* current_name,
                           std::string& dir_path, std::string& native_name);*/

  sset& files;
  sset& badFiles;
  std::size_t include_depth;
  //std::string fileNameBuf;
};

void FileProcessor::processFile(std::string_view file,
                                std::vector<std::string>& includeDirs) {
  std::cout << "Start scanning source file [" << file << "]" << std::endl;
  boost::wave::util::file_position_type scanPos;  // used in catch blocks
  try {
    std::ifstream in(file.data());

    if (!in.is_open()) {
      std::cerr << "Could not open input file [" << file << "]" << std::endl;
      return;
    }

    in.unsetf(std::ios::skipws);
    std::string buf{std::istreambuf_iterator<char>(in.rdbuf()),
                    std::istreambuf_iterator<char>()};

    typedef boost::wave::cpplexer::lex_iterator<
        boost::wave::cpplexer::lex_token<> >
        lex_iterator_type;
    typedef boost::wave::context<
        std::string::iterator, lex_iterator_type,
        boost::wave::iteration_context_policies::load_file_to_string,
        trace_include_files>
        context_type;

    sset files;
    sset badFiles;
    trace_include_files trace(files, badFiles);

    context_type ctx(buf.begin(), buf.end(), file.data(), trace);

    // add include directories to the include path

    for (const auto& s : includeDirs) ctx.add_include_path(s.c_str());

    /*// add system include directories to the include path
    if (vm.count("sysinclude")) {
      vector<string> const& syspaths = vm["sysinclude"].as<vector<string> >();
      vector<string>::const_iterator end = syspaths.end();
      for (vector<string>::const_iterator cit = syspaths.begin(); cit != end;
           ++cit) {
        ctx.add_sysinclude_path((*cit).c_str());
      }
    }*/

    // analyze the actual file
    context_type::iterator_type first = ctx.begin();
    context_type::iterator_type last = ctx.end();

    std::cout << "Printing dependency information for: " << file << std::endl;

    while (first != last) {
      scanPos = (*first).get_position();
      ++first;
    }

    // prepend endl before next file
    std::cout << std::endl;

    /* context_type ctx{buf.begin(), buf.end()};

    context_type::iterator_type first{ctx.begin()};
    context_type::iterator_type last{ctx.end()};

    while (first != last) {
      scanPos = (*first).get_position();
      std::cout << (*first).get_value();
      ++first;
    }*/
  } catch (boost::wave::cpp_exception const& e) {
    std::cerr << e.file_name() << "(" << e.line_no() << "): " << e.description()
              << std::endl;
  } catch (std::exception const& e) {
    std::cerr << scanPos.get_file() << "(" << scanPos.get_line() << "): "
              << "exception caught: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << scanPos.get_file() << "(" << scanPos.get_line() << "): "
              << "unexpected exception caught." << std::endl;
  }
};