#include "hamming_codes.h"

#include <iomanip>

const uint8_t kNumberOfMeasureUnits = 5;
std::string measure_types[kNumberOfMeasureUnits] = {"Bytes", "Kb", "Mb", "Gb", "Tb"};

std::pair<uint8_t, double> GetMeasureUnit(size_t size) {
  uint8_t pow = 0;
  double size_double = static_cast<double>(size);
  while (size_double / (1 << 10) > 1) {
    pow++;
    size_double /= (1 << 10);
  }

  return {pow, size_double};
}

bool Archiver::SearchInfo(std::vector<std::tuple<std::string, double, uint8_t>>& files, 
                          const std::string& filename) {
  path_ = filename;
  std::ifstream file(filename, std::ios::in | std::ios::binary);

  bool check_result = CheckArchiver(file);
  if (!check_result) {

    return false;
  }

  uint8_t number_of_parity_bits = GetTwoBytes(file);
  files.push_back(std::make_tuple("Ham arc", 0, number_of_parity_bits));

  bool compress = GetTwoBytes(file);
  files.push_back(std::make_tuple("Compress", 0, compress));
  if (compress) {
    // skip tree
    std::map<uint8_t, std::vector<bool>> tree;
    ReadTree(file, tree);
  }

  std::string current_filename;
  uint8_t ch;
  while(file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
    uint16_t filename_length = GetFilenameSize(ch, file);

    uint8_t* filename_bytes = new uint8_t[filename_length];
    ReadBytes(file, filename_bytes, filename_length);
    
    byte_ = 0;
    index_byte_ = 0;
    index_ = 0;
    std::string current_filename = "";
    Decode(filename_bytes, filename_length, number_of_parity_bits, current_filename, false);
    delete[] filename_bytes;
    size_t data_size = GetDataSize(file, kDataSize);
    
    size_t data_size_without_parity = CountBytesWithoutParityBits(data_size, number_of_parity_bits);
    std::pair<uint8_t, double> measure_unit = GetMeasureUnit(data_size_without_parity);
    files.push_back(std::make_tuple(current_filename, measure_unit.second, measure_unit.first));

    size_t pos = file.tellg();
    file.seekg(pos + data_size);
  }

  return true;
}

void Archiver::Info(const std::string& filename) {
  std::vector<std::tuple<std::string, double, uint8_t>> files_list;
  bool search_result = SearchInfo(files_list, filename);
  if (!search_result) {
    return;
  }
  for (const auto& file: files_list) {
    if (std::get<0>(file) == "Ham arc") {
      std::cout << "Archiver with hamming codes\n";
      std::cout << "Number of parity bits: " << static_cast<int16_t>(std::get<2>(file)) << "\n";
    }
    else if (std::get<0>(file) == "Compress") {
      std::cout << "Compression used: ";
      if (std::get<2>(file)) {
        std::cout << "yes\n";
      }
      else {
        std::cout << "no\n";
      }
    }
    else {
      std::cout << std::setprecision(3) << std::get<0>(file) << "\t";
      std::cout << std::get<1>(file) << " " << measure_types[std::get<2>(file)] << "\n";
    }
  }
}
