#include "Argument.h"
#include "HelpLine.h"

#include <charconv>

namespace ArgumentParser {

  namespace ErrorMessages {
    const std::string kEmptyArguments = "Enter arguments please";
    const std::string kSeveralStringPositionalArguemnts = "There are can be only one string positional argument";
    const std::string kNoStringPositionalArguemnt = "There are not any string positional argument";
    const std::string kSeveralIntPositionalArguemnts = "There are can be only one int positional argument";
    const std::string kNoIntPositionalArguemnt = "There are not any integer positional argument";
    const std::string kArgumentDoesNotExist = "Argument with such name does not exist";
    const std::string kPositionalArgumentDoesNotExist = "Positional argument with such name does not exist";
    const std::string kIndexExceeding = "Index exceeds the number of value";
  }
  
  void EndProgramWithError(const std::string& error_message);

  class ArgParser {
  public:
    ArgParser() = default;
    ArgParser(const std::string& name);
    
    void AddStringArgument(const Argument<std::string>& argument);
    Argument<std::string>& AddStringArgument(const std::string& name);
    Argument<std::string>& AddStringArgument(char short_name,
                                             const std::string& name);
    Argument<std::string>& AddStringArgument(const std::string& name,
                                             const std::string& description);
    Argument<std::string>& AddStringArgument(char short_name, 
                                             const std::string& name, 
                                             const std::string& description);

    void AddIntArgument(const Argument<int32_t>& argument);
    Argument<int32_t>& AddIntArgument(const std::string& name);
    Argument<int32_t>& AddIntArgument(char short_name, 
                                      const std::string& name);
    Argument<int32_t>& AddIntArgument(const std::string& name, 
                                      const std::string& description);
    Argument<int32_t>& AddIntArgument(char short_name, 
                                      const std::string& name,
                                      const std::string& description);

    void AddFlagArgument(const Argument<bool>& argument);
    Argument<bool>& AddFlag(const std::string& name);
    Argument<bool>& AddFlag(char short_name, 
                            const std::string& name);
    Argument<bool>& AddFlag(const std::string& name,
                            const std::string& description);
    Argument<bool>& AddFlag(char short_name, 
                            const std::string& name,
                            const std::string& description);

    template<typename T>
    void SetArgument(const T& name, const std::string& value) {
      if (IsDigitArgument(value)) {
        SetIntArgument(name, std::stoi(value));
      }
      else {
        SetStringArgument(name, value);
      }
    }

    template<typename T>
    void SetArgument(std::vector<Argument<T>>& array, const std::string& name, const T& value) {
      for (auto& argument: array) {
        if (argument.argument_name == name) {
          argument.Set(value);
          break;
        }
      }
    }

    template<typename T>
    void SetArgument(std::vector<Argument<T>>& array, char short_name, const T& value) {
      for (auto& argument: array) {
        if (argument.argument_short_name == short_name) {
          argument.Set(value);
          break;
        }
      }
    }

    void SetStringArgument(const std::string& name, const std::string& value);
    void SetStringArgument(char short_name, const std::string& value);

    void SetIntArgument(const std::string& name, int32_t value);
    void SetIntArgument(char short_name, int32_t value);

    void SetFlag(const std::string& name);
    void SetFlag(char short_name);

    template<typename T>
    int32_t GetIndexPositionalArgument(std::vector<Argument<T>>& array) const {
      int32_t index = -1;
      for (int i = 0; i < array.size(); i++) {
        if (array[i].flag_positional) {
          if (index == -1) {
            index = i;
          }
          else {
            EndProgramWithError(ErrorMessages::kSeveralStringPositionalArguemnts);
          }
        }
      }
      if (index == -1) {
        EndProgramWithError(ErrorMessages::kNoStringPositionalArguemnt);
      }

      return index;
    }

    void SetPositionalArgument(const std::string& value);

    template<typename T>
    void SetPositionalArgument(std::vector<Argument<T>>& array, int32_t index, const T& value) {
       if (array[index].flag_multi_values) {
        array[index].argument_values->push_back(value);
      }
      else {
        *array[index].argument_value = value;
      }
    }

    void SetStringPositionalArgument(const std::string& value);
    void SetIntPositionalArgument(int32_t value);

    template<typename T>
    T GetValue(const std::vector<Argument<T>>& array, const std::string& name) const {
      for (const auto& argument: array) {
        if (argument.argument_name == name) {

          return *argument.argument_value;
        }
      }

      EndProgramWithError(ErrorMessages::kArgumentDoesNotExist);
    }

    template<typename T>
    T GetValue(const std::vector<Argument<T>>& array, const std::string& name, int32_t position) const {
      for (const auto& argument: array) {
        if (argument.flag_multi_values && argument.argument_name == name) {
          if (argument.argument_values->size() > position) {

            return (*argument.argument_values)[position];
          }
          else {

            EndProgramWithError(ErrorMessages::kIndexExceeding);
          }
        }
      }

      EndProgramWithError(ErrorMessages::kPositionalArgumentDoesNotExist);
    }

    std::string GetStringValue(const std::string& name) const;
    std::string GetStringValue(const std::string& name, int32_t position) const;

    int32_t GetIntValue(const std::string& name) const;
    int32_t GetIntValue(const std::string& name, int32_t position) const;

    bool GetFlag(const std::string& name) const;

    void AddHelp(char short_name, const std::string& name, const std::string& description);
    bool Help() const;
    std::string HelpDescription() const;

    bool CheckNamedArgument(const std::string& argument) const;
    void SetNamedArgument(const std::string& argument);
 
    bool CheckArguments() const;
    bool Parse(int32_t argc, char** argv);
    bool Parse(std::vector<std::string> args);

    template<typename T>
    void AddReference(std::vector<Argument<T>>& array, T default_value) {
      size_t last_index = array.size() - 1;
      array[last_index].saved_value = default_value;
      array[last_index].argument_value = &array[last_index].saved_value;
    }

    template<typename T>
    void AddMultiValueReference(std::vector<Argument<T>>& array) {
      size_t last_index = array.size() - 1;
      array[last_index].saved_values = {};
      array[last_index].argument_values = &array[last_index].saved_values;
    }

    bool IsDigitArgument(const std::string& s);

    ~ArgParser() = default;
    
  private:
    std::string parser_name_;
    std::string app_name_;
    bool help_command_ = false;
    int32_t index_string_positional_argument_ = std::numeric_limits<int32_t>::min();
    int32_t index_integer_positional_argument_ = std::numeric_limits<int32_t>::min();
    std::vector<Argument<std::string>> string_arguments_;
    std::vector<Argument<int32_t>> integer_arguments_;
    std::vector<Argument<bool>> flag_arguments_;
    std::vector<HelpLine> help_;
};

} // namespace ArgumentParser
