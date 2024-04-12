#pragma once
#include <iostream>

struct Options {
  const char* input_filename;
  const char* output_directory;
  int64_t max_iterations = std::numeric_limits<uint64_t>::max();
  int64_t save_frequency = std::numeric_limits<uint64_t>::max();
};

enum class ArgParseResultType {
  UnknowArgument,
  IncorrectIterationsCount,
  IncorrectFrequency,
  CannotOpenFile,
  Ok
};

struct ArgParseResult {
  Options options;
  ArgParseResultType type;

  ArgParseResultType ParseArguments(int32_t argc, char** argv);
};
