#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>


class CoutSentry {
  std::ostringstream buffer_stream_;
  std::streambuf* old_buffer_;

public:
  CoutSentry() : old_buffer_(std::cout.rdbuf()) { std::cout.rdbuf(buffer_stream_.rdbuf()); }
  ~CoutSentry() { std::cout.rdbuf(old_buffer_); }

  CoutSentry(const CoutSentry&) = delete;
  void operator=(const CoutSentry&) = delete;

  std::string str() const { return buffer_stream_.str(); }
};


class FileSentry {
  const char* filename_;

public:
  explicit FileSentry(const char* filename) : filename_(filename) {
    std::remove(filename_);
  }
  ~FileSentry() {
    std::remove(filename_);
  }

  FileSentry(const FileSentry&) = delete;
  void operator=(const FileSentry&) = delete;

  std::string str() const {
    std::ifstream file{filename_};
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
  }
};
