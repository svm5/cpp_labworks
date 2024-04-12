#include "archiver_extract.cpp"

void Archiver::DeleteFiles(const std::string& archiver_name, const std::vector<std::string>& names) {
  ExtractFiles(archiver_name, names, true);
}
