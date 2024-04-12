#include "parse_arguments.h"

#include <cstring>
#include <cstdlib>
#include <fstream>

namespace ArgumentNames {
  const char* InputFilename = "--input";
  const char* InputFilenameShort = "-i";
  const char* OutputDirectory = "--output";
  const char* OutputDirectoryShort = "-o";
  const char* MaxIterations = "--max-iter";
  const char* MaxIterationsShort = "-m";
  const char* SaveFrequency = "--freq";
  const char* SaveFrequencyShort = "-f";
}

namespace ErrorMessages {
  const char* UnknowArgument = "unknow argument";
  const char* IncorrectIterationsCount = "Iterations max count must be a positive integer number";
  const char* IncorrectFrequency = "Frequancy must be not negative integer number";
  const char* CannotOpenFile = "Cannot open file and read data";
}

enum class ArgumentTypes {
    Nothing,
    InputFilename,
    OutputDirectory,
    MaxIterations,
    SaveFrequency
};

void EndProgramWithError(ArgParseResultType type) {
  switch (type)
  {
  case ArgParseResultType::CannotOpenFile:
    std::cerr << ErrorMessages::CannotOpenFile;
    break;

  case ArgParseResultType::IncorrectIterationsCount:
    std::cerr << ErrorMessages::IncorrectIterationsCount;
    break;

  case ArgParseResultType::IncorrectFrequency:
    std::cerr << ErrorMessages::IncorrectFrequency;
    break;

  case ArgParseResultType::UnknowArgument:
    std::cerr << ErrorMessages::UnknowArgument;
    break;
  
  default:
    break;
  }
  exit(EXIT_SUCCESS);
}

bool CheckInputFilename(const char* filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return false;
  }

  return true;
}

bool CheckIntegerArgument(const char* integer_argument) {
  int16_t index = 0;
  while (integer_argument[index] != '\0') {
    if (!std::isdigit(integer_argument[index])) {

      return false;
    }
    ++index;
  }

  return true;
}

bool CompareArgumnts(const char* current, const char* full_version, const char* short_version) {
  return (strcmp(current, full_version) == 0) || (strcmp(current, short_version) == 0);
}

ArgParseResultType ArgParseResult::ParseArguments(int argc, char** argv) {
  ArgumentTypes prev_argument = ArgumentTypes::Nothing;
  char* current_argument;
  for (int i = 1; i < argc; i++) {
    current_argument = argv[i];
    if (CompareArgumnts(current_argument, ArgumentNames::InputFilename, ArgumentNames::InputFilenameShort)) {
      prev_argument = ArgumentTypes::InputFilename;
    } else if (CompareArgumnts(current_argument, ArgumentNames::OutputDirectory, ArgumentNames::OutputDirectoryShort)) {
      prev_argument = ArgumentTypes::OutputDirectory;
    } else if (CompareArgumnts(current_argument, ArgumentNames::MaxIterations, ArgumentNames::MaxIterationsShort)) {
      prev_argument = ArgumentTypes::MaxIterations;
    } else if (CompareArgumnts(current_argument, ArgumentNames::SaveFrequency, ArgumentNames::SaveFrequencyShort)) {
      prev_argument = ArgumentTypes::SaveFrequency;
    } else {
      if (prev_argument == ArgumentTypes::InputFilename) {
        if (!CheckInputFilename(current_argument)) {
          type = ArgParseResultType::CannotOpenFile;

          return type;
        }
        options.input_filename = current_argument;
      } else if (prev_argument == ArgumentTypes::OutputDirectory) {
        options.output_directory = current_argument;
      } else if (prev_argument == ArgumentTypes::MaxIterations) {
        if (!CheckIntegerArgument(current_argument)) {
          type = ArgParseResultType::IncorrectIterationsCount;

          return type;
        }
        options.max_iterations = std::atoi(current_argument);
      } else if (prev_argument == ArgumentTypes::SaveFrequency) {
        if (!CheckIntegerArgument(current_argument)) {
          type = ArgParseResultType::IncorrectFrequency;

          return type;
        }
        options.save_frequency = std::atoi(current_argument);
      }
      else {
        return ArgParseResultType::UnknowArgument;
      }
      prev_argument = ArgumentTypes::Nothing;
    }
  }
  
  current_argument = nullptr;
  delete current_argument;
 
  type = ArgParseResultType::Ok;
  return type;
}
