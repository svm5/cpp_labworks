#include "ArgParser.h"

#include "HelpLine.cpp"

std::pair<std::string, std::string> SplitBy(const std::string& s, const std::string& delimiter) {
  size_t delimiter_position = s.find(delimiter);
  std::string first = s.substr(0, delimiter_position);
  std::string second = s.substr(delimiter_position + delimiter.length(), s.length());

  return std::make_pair(first, second);
}

void CutPrefix(std::string& s, const std::string& prefix) {
  s.erase(0, prefix.length());
}

namespace ArgumentParser {

  void EndProgramWithError(const std::string& error_message) {
    std::cerr << error_message << "\n";
    exit(EXIT_SUCCESS);
  }

  ArgParser::ArgParser(const std::string& name)
    : parser_name_(name)
    {}

  void ArgParser::AddStringArgument(const Argument<std::string>& argument) {
    string_arguments_.push_back(argument);
    std::string s;
    AddReference(string_arguments_, s);
    AddMultiValueReference(string_arguments_);
  }

  Argument<std::string>& ArgParser::AddStringArgument(const std::string& name) {
    AddStringArgument(Argument<std::string>(name));

    return string_arguments_.back();
  }

  Argument<std::string>& ArgParser::AddStringArgument(char short_name,
                                                      const std::string& name) {
    AddStringArgument(Argument<std::string>(short_name, name));

    return string_arguments_.back();
  }

  Argument<std::string>& ArgParser::AddStringArgument(const std::string& name,
                                           const std::string& description) {
    AddStringArgument(Argument<std::string>(name, description));

    return string_arguments_.back();
  }

  Argument<std::string>& ArgParser::AddStringArgument(char short_name, 
                                                      const std::string& name, 
                                                      const std::string& description) {
    AddStringArgument(Argument<std::string>(short_name, name, description));

    return string_arguments_.back();                                          
  }

  void ArgParser::AddIntArgument(const Argument<int32_t>& argument) {
    integer_arguments_.push_back(argument);
    AddReference(integer_arguments_, 0);
    AddMultiValueReference(integer_arguments_);
  }

  Argument<int32_t>& ArgParser::AddIntArgument(const std::string& name) {
    AddIntArgument(Argument<int32_t>(name));

    return integer_arguments_.back();
  }

  Argument<int32_t>& ArgParser::AddIntArgument(char short_name, 
                                               const std::string& name) {
    AddIntArgument(Argument<int32_t>(short_name, name));

    return integer_arguments_.back();
  }

  Argument<int32_t>& ArgParser::AddIntArgument(const std::string& name, 
                                               const std::string& description) {
    AddIntArgument(Argument<int32_t>(name, description));

    return integer_arguments_.back();
  }

  Argument<int32_t>& ArgParser::AddIntArgument(char short_name, 
                                               const std::string& name,
                                               const std::string& description) {
    AddIntArgument(Argument<int32_t>(short_name, name, description));

    return integer_arguments_.back();
  }

  void ArgParser::AddFlagArgument(const Argument<bool>& argument) {
    flag_arguments_.push_back(argument);
    AddReference(flag_arguments_, false);
  }

  Argument<bool>& ArgParser::AddFlag(const std::string& name) {
    AddFlagArgument(Argument<bool>(name));

    return flag_arguments_.back();
  }

  Argument<bool>& ArgParser::AddFlag(char short_name, 
                                     const std::string& name) {
    AddFlagArgument(Argument<bool>(short_name, name));

    return flag_arguments_.back();
  }

  Argument<bool>& ArgParser::AddFlag(const std::string& name,
                          const std::string& description) {
    AddFlagArgument(Argument<bool>(name, description));

    return flag_arguments_.back();
  }

  Argument<bool>& ArgParser::AddFlag(char short_name, 
                                     const std::string& name,
                                     const std::string& description) {
    AddFlagArgument(Argument<bool>(short_name, name, description));

    return flag_arguments_.back();
  }

  void ArgParser::SetStringArgument(const std::string& name, const std::string& value) {

    SetArgument(string_arguments_, name, value);
  }

  void ArgParser::SetStringArgument(char short_name, const std::string& value) {
    
    SetArgument(string_arguments_, short_name, value);
  }

  void ArgParser::SetIntArgument(const std::string& name, int32_t value) {
    
    SetArgument(integer_arguments_, name, value);
  }

  void ArgParser::SetIntArgument(char short_name, int32_t value) {
    
    SetArgument(integer_arguments_, short_name, value);
  }

  void ArgParser::SetFlag(const std::string& name) {
    for (auto& flag: flag_arguments_) {
      if (flag.argument_name == name) {
        flag.Set();
      }
    }
  }

  void ArgParser::SetFlag(char short_name) {
    for (auto& flag: flag_arguments_) {
      if (flag.argument_short_name == short_name) {
        flag.Set();
      }
    }
  }

  void ArgParser::SetPositionalArgument(const std::string& value) {
    if (IsDigitArgument(value)) {
      SetIntPositionalArgument(std::stoi(value));
    }
    else {
      SetStringPositionalArgument(value);
    }
  }

  void ArgParser::SetStringPositionalArgument(const std::string& value) {
    if (index_string_positional_argument_ == std::numeric_limits<int32_t>::min()) {
      index_string_positional_argument_ = GetIndexPositionalArgument(string_arguments_);
    }
    SetPositionalArgument(string_arguments_, index_string_positional_argument_, value);
  }

  void ArgParser::SetIntPositionalArgument(int32_t value) {
    if (index_integer_positional_argument_ == std::numeric_limits<int32_t>::min()) {
      index_integer_positional_argument_ = GetIndexPositionalArgument(integer_arguments_);
    }
    SetPositionalArgument(integer_arguments_, index_integer_positional_argument_, value);
  }

  std::string ArgParser::GetStringValue(const std::string& name) const {

    return GetValue(string_arguments_, name);
  }

  std::string ArgParser::GetStringValue(const std::string& name, int32_t position) const {
    
    return GetValue(string_arguments_, name, position);
  }

  int32_t ArgParser::GetIntValue(const std::string& name) const {
    
    return GetValue(integer_arguments_, name);
  }

  int32_t ArgParser::GetIntValue(const std::string& name, int32_t position) const {
    
    return GetValue(integer_arguments_, name, position);
  }

  bool ArgParser::GetFlag(const std::string& name) const {
    
    return GetValue(flag_arguments_, name);
  }

  void ArgParser::AddHelp(char short_name, const std::string& name, const std::string& description) {
    help_.push_back(HelpLine(short_name, name, description));
  }

  bool ArgParser::Help() const {

    return help_command_;
  }

  std::string ArgParser::HelpDescription() const {
    std::stringstream result;
    result << parser_name_ << "\n";
    result << "Description\n\n";
    for (int i = 0; i < help_.size(); i++) {
      result << help_[i].CreateOutput();
      result << "\n";
    }
    
    return result.str();
  }

  bool ArgParser::CheckNamedArgument(const std::string& argument) const {

    return argument.find("=") != std::string::npos; 
  }

  void ArgParser::SetNamedArgument(const std::string& argument) {
    std::pair<std::string, std::string> split_result = SplitBy(argument, "="); 
    std::string name = split_result.first;
    std::string value = split_result.second;
    
    if (argument.starts_with("--")) {
      // arguments like --param=something_str_or_int
      CutPrefix(name, "--");
      SetArgument(name, value);
    }
    else {
      // arguments like -p=something_str_or_int
      CutPrefix(name, "-");
      SetArgument(name[0], value);
    }
  }

  bool ArgParser::CheckArguments() const {
    for (const auto& argument: string_arguments_) {
      if (!argument.CheckArgument()) {

        return false;
      }
    }
    for (const auto& argument: integer_arguments_) {
      if (!argument.CheckArgument()) {

        return false;
      }
    }

    return true;
  }

  bool ArgParser::Parse(int32_t argc, char** argv) {
    std::vector<std::string> string_argv;
    for (int i = 0 ; i < argc; i++) {
      string_argv.push_back(static_cast<std::string>(argv[i]));
    }

    return Parse(string_argv);
  }

  bool ArgParser::Parse(std::vector<std::string> args) {
    if (!args.size()) {
      EndProgramWithError(ErrorMessages::kEmptyArguments);
    }
    app_name_ = args[0];

    for (int i = 1; i < args.size(); i++) {
      std::string current_argument = args[i];
      if (current_argument == "--help") {
        help_command_ = true;

        return true;
      }
      else if (CheckNamedArgument(current_argument)) {
        // arguments like --param=123 or -p=abc
        SetNamedArgument(current_argument);
      }
      else if (current_argument.starts_with("--")) {
        // flags like --flag
        CutPrefix(current_argument, "--");
        SetFlag(current_argument);
      }
      else if (current_argument.starts_with("-") 
               && current_argument.length() > 1 
               && !isdigit(current_argument[1])) {
        // flags like -ac
        CutPrefix(current_argument, "-");
        for (int i = 0; i < current_argument.length(); i++) {
          SetFlag(current_argument[i]);
        }
      }
      else {
        SetPositionalArgument(current_argument);
      }
    }
    
    return CheckArguments();
  }

  bool ArgParser::IsDigitArgument(const std::string& s) {
    int32_t start_index = 0;
    if (s.length() > 0 && s[0] == '-') {
      start_index++;
    }
    for (int i = start_index; i < s.length(); i++) {
      if (!isdigit(s[i])) {

        return false;
      }
    }

    return true;
  }
}
