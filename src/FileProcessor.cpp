#include "FileProcessor.hpp"

#include <boost/wave.hpp>
#include <boost/wave/cpp_exceptions.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>
#include <boost/wave/preprocessing_hooks.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string_view>
#include <typeinfo>

// Some hints:
// https://github.com/boostorg/wave/blob/develop/samples/quick_start/quick_start.cpp
// https://stackoverflow.com/questions/9449887/removing-c-c-style-comments-using-boostregex
// https://github.com/boostorg/wave/tree/develop/samples/list_includes

// custom preprocess hooks
struct trace_include_files
    : public boost::wave::context_policies::default_preprocessing_hooks {
  trace_include_files(std::vector<pairs>& data)
      : m_data(data), m_includeDepth(1) {}

  template <typename CtxType>
  void opened_include_file(CtxType const& ctx, std::string const& relname,
                           std::string const& filename,
                           bool is_system_include) {
    std::filesystem::path p{filename};
    std::string fileOnlyName{p.filename().string()};
    submitFilename(fileOnlyName);
    ++m_includeDepth;
  }

  template <typename CtxType>
  void returning_from_include_file(CtxType const& ctx) {
    --m_includeDepth;
  }

  template <typename CtxType, typename ExceptionType>
  void throw_exception(CtxType const& ctx, ExceptionType const& e) {
    // skip bad include file
    if (typeid(ExceptionType) == typeid(boost::wave::preprocess_exception) &&
        e.get_errorcode() ==
            boost::wave::preprocess_exception::error_code::bad_include_file) {
      // parse message
      std::string foo = e.description();
      std::size_t pos = foo.find_last_of(" ");  // maybe regexp will be better
      if (pos == std::string::npos)
        pos = 0;  // print entire message if there are no spaces
      submitFilename(foo.substr(pos + 1, foo.size() - pos) + " (!) <" +
                     foo.substr(0, pos - 1) + ">");
    } else {
      boost::throw_exception(e);
    }
  }

  std::vector<pairs>& m_data;
  int16_t m_includeDepth;

  void submitFilename(std::string_view fileName) {
    pairs foo{std::make_pair(fileName, m_includeDepth)};
    m_data.push_back(std::move(foo));
  };
};

FileData FileProcessor::_worker(std::string_view file,
                            std::vector<std::string>& includeDirs) {
  boost::wave::util::file_position_type scanPos;  // used in catch blocks
  FileData out;
  out.fileName = file;
  try {
    std::ifstream in(file.data());

    if (!in.is_open()) {
      std::cerr << "Could not open input file [" << file << "]" << std::endl;
      return out;
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

    trace_include_files trace(out.includes);
    context_type ctx(buf.begin(), buf.end(), file.data(), trace);

    for (const auto& s : includeDirs) ctx.add_include_path(s.c_str());

    context_type::iterator_type first = ctx.begin();
    context_type::iterator_type last = ctx.end();

    while (first != last) {
      scanPos = (*first).get_position();
      ++first;
    }

  } catch (boost::wave::cpp_exception const& e) {
    std::cerr << e.file_name() << "(" << e.line_no() << "): " << e.description()
              << std::endl;
  } catch (std::exception const& e) {
    std::cerr << scanPos.get_file() << "(" << scanPos.get_line() << "): "
              << "An exception caught: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << scanPos.get_file() << "(" << scanPos.get_line() << "): "
              << "An unexpected exception caught!" << std::endl;
  }
  return out;
};

void FileProcessor::processFile(std::string_view file,
                                std::vector<std::string>& includeDirs) {
  try {
    auto futureHandler = std::async(std::launch::async, &FileProcessor::_worker,
                                    this, file, std::ref(includeDirs));

    m_resultHandlers.push_back(std::move(futureHandler));
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
  }
}

void FileProcessor::waitUntilResults() {
  try {
    for (auto& f : m_resultHandlers) m_fileData.push_back(std::move(f.get()));
    m_resultHandlers.clear();
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
  }
}

std::vector<FileData>&& FileProcessor::takeResults() {
  return std::move(m_fileData);
}