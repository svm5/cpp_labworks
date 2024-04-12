#include "HelpLine.h"

#include <sstream>

HelpLine::HelpLine(char short_name, 
                    const std::string& name, 
                    const std::string& description)
  : argument_short_name(short_name)
  , argument_name(name)
  , argument_description(description)
  {}

std::string HelpLine::CreateOutput() const {
  std::stringstream result;
  result <<  "Argument name: " << argument_name;
  result << ", short name: ";
  result << argument_short_name;
  result << ", description: " + argument_description;
  result << "\n";

  return result.str();
}