#pragma once

#include <iostream>
#include <string>

struct HelpLine {

  HelpLine() = default;

  char argument_short_name;
  std::string argument_name;
  std::string argument_description;

  HelpLine(char short_name,
            const std::string& name,
            const std::string& description);
 
  std::string CreateOutput() const;

  ~HelpLine() = default;
};
