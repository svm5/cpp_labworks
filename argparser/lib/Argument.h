#pragma once

#include <iostream>
#include <vector>
#include <string>

template <typename T> struct Argument {
    char argument_short_name = '-';
    std::string argument_name;
    std::string argument_description;

    T saved_value;
    T* argument_value;

    std::vector<T> saved_values;
    std::vector<T>* argument_values;

    bool flag_used = false;
    
    bool flag_positional = false;

    bool flag_multi_values = false;
    size_t values_minimum_count = 0;

    Argument()
      : argument_value(&saved_value)
      , argument_values(&saved_values)
      {}

    Argument(const std::string& name)
      : argument_name(name)
      , argument_value(&saved_value)
      , argument_values(&saved_values)
      {}

    Argument(char short_name, const std::string& name)
      : argument_short_name(short_name)
      , argument_name(name)
      {}

    Argument(const std::string& name,
             const std::string& description)
      : argument_name(name)
      , argument_description(description)
      {}

    Argument(char short_name,
             const std::string& name,
             const std::string& description)
      : argument_short_name(short_name)
      , argument_name(name)
      , argument_description(description)
      {}
 
    Argument<T>& Default(const T& value) {
      *argument_value = value;
      flag_used = true;

      return *this;
    }

    Argument<T>& Default(const std::vector<T>& values) {
      *argument_values = values;
      flag_used = true;

      return *this;
    }

    Argument<T>& StoreValue(T& value) {
      argument_value = &value;

      return *this;
    }

    Argument<T>& StoreValues(std::vector<T>& values) {
      argument_values = &values;

      return *this;
    }

    Argument<T>& MultiValue() {
      flag_multi_values = true;

      return *this;
    }

    Argument<T>& MultiValue(size_t min_args_count) {
      flag_multi_values = true;
      values_minimum_count = min_args_count;

      return *this;
    }

    Argument<T>& Positional() {
      flag_positional = true;

      return *this;
    }

    void Set(const T& value) {
      flag_used = true;
      if (flag_multi_values) {
        argument_values->push_back(value);
      }
      else {
        *argument_value = value;
      }
    }

    bool CheckArgument() const {
      if (flag_multi_values) {

        return argument_values->size() >= values_minimum_count;
      }

      return flag_used;
    }

    ~Argument() = default;
  };

  template <> struct Argument<bool> {
    char argument_short_name = '-';
    std::string argument_name;
    std::string argument_description;

    bool saved_value = false;

    bool* argument_value;
    
    bool flag_positional = false;

    Argument() = default;
    Argument(const std::string& name)
      : argument_name(name)
      {}

    Argument(char short_name, const std::string& name)
      : argument_short_name(short_name)
      , argument_name(name)
      {}

    Argument(const std::string& name,
             const std::string& description)
      : argument_name(name)
      , argument_description(description)
      {}

    Argument(char short_name,
             const std::string& name,
             const std::string& description)
      : argument_short_name(short_name)
      , argument_name(name)
      , argument_description(description)
      {}

    Argument<bool>& Default(bool value) {
      *argument_value = value;

      return *this;
    }

    Argument<bool>& StoreValue(bool& value) {
      argument_value = &value;

      return *this;
    }

    void Set() {
      *argument_value = true;
    }

    ~Argument() = default;
  };
  