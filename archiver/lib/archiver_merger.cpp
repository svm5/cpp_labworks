#include "hamming_codes.h"

bool CheckArchives(std::ifstream& first_archive, std::ifstream& second_archive) {
  if (!first_archive.is_open()) {
    std::cout << "Cannot open the first file;(\n";

    return false;
  }

  if (!second_archive.is_open()) {
    std::cout << "Cannot open the second file;(\n";

    return false;
  }

  bool is_first_archiver = CheckType(first_archive);
  if (!is_first_archiver) {
    std::cout << "First file is not archiver\n";

    return false;
  } 

  bool is_second_archiver = CheckType(second_archive);
  if (!is_second_archiver) {
    std::cout << "Second file is not archiver\n";

    return false;
  }

  return true;
}

void Archiver::Merge(const std::string& new_name, const std::string& first, const std::string& second) {
  std::ifstream first_archive(first, std::ios::in | std::ios::binary);
  std::ifstream second_archive(second, std::ios::in | std::ios::binary);

  bool result_check_archives = CheckArchives(first_archive, second_archive);
  if (!result_check_archives) {
    return;
  }

  uint8_t first_parity = GetTwoBytes(first_archive);
  uint8_t second_parity = GetTwoBytes(second_archive);

  bool first_compress = GetTwoBytes(first_archive);
  bool second_compress = GetTwoBytes(second_archive);

  if (first_parity != second_parity || first_compress || second_compress) {
    std::cout << "Cannot merge archivers\n";

    return;
  }

  // new archiver
  WriteType(new_name, kType);
  // write number parity bits
  WriteTwoParts(new_name, first_parity);
  WriteTwoParts(new_name, first_compress);

  std::ofstream new_archive(new_name, std::ios::app | std::ios::binary);

  // write data from first archive, after that from second
  uint8_t ch;
  while (first_archive.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
    new_archive.write(reinterpret_cast<char*>(&ch), sizeof(ch));
  }
  
  while(second_archive.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
    new_archive.write(reinterpret_cast<char*>(&ch), sizeof(ch));
  }

  new_archive.close();
}
