#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

class Archiver {
public:
  // sizes (after add parity bits) of blocks 
  const std::string kType = "HF";
  static const uint8_t kObjectType = 4;  // two symbols - HF - .haf archiver
  static const uint8_t kNumberOfParityBits = 2;
  static const uint8_t kFilenameLength = 2;
  static const uint8_t kDataSize = 4;  // how many bytes in file

  static const int32_t kDataBlockSize = 100000; 
  
  Archiver() = default;

  Archiver(const std::string& path);

  Archiver(uint16_t parity_bits, const std::string& path);

  uint8_t GetNumberOfParityBits(const std::string& archiver_name);

  // void ReadTree(const std::string& archiver_name);

  void AddByte(int32_t block, int& cnt);

  void AddLast();

  void Encode(uint8_t* bytes, int32_t size);
  void EncodeWithCompress(uint8_t* bytes, int32_t size, const std::map<uint8_t, std::vector<bool>>& tree);
  
  void Decode(uint8_t* bytes, int32_t size, uint8_t count_of_parity_bits,
              std::string& filename, bool flag_write);
  void DecodeWithCompress(uint8_t* bytes, int32_t size, uint8_t count_of_parity_bits,
                          std::string& filename, bool flag_write, 
                          const std::map<uint8_t, std::vector<bool>>& tree);

  bool SearchInfo(std::vector<std::tuple<std::string, double, uint8_t>>& files, const std::string& filename);

  void Info(const std::string& filename);

  void Create();
  void Create(const std::string& archiver_name, 
              uint8_t number_of_parity_bits, 
              const std::vector<std::string>& filenames,
              bool compress);

  void AddFile(const std::string& archiver_name, 
               uint8_t number_of_parity_bits, 
               const std::string& filename);

  void Merge(const std::string& new_name, const std::string& first, const std::string& second);

  bool CheckArchiver(std::ifstream& file);

  void ExtractAll(const std::string& archiver_name);

  void ExtractFiles(const std::string& archiver_name, const std::vector<std::string>& names, bool flag_delete);

  void DeleteFiles(const std::string& archiver_name, const std::vector<std::string>& names);

  void Reset();

  ~Archiver() = default;
private:
  uint8_t parity_bits_;
  std::string path_;
  std::vector<bool> bits_;

  size_t index_ = 0;
  uint8_t byte_ = 0;
  size_t index_byte_ = 0;
};