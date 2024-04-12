#include "hamming_codes.h"

#include <cstdio>
#include <cstdlib>

void Archiver::ExtractAll(const std::string& archiver_name) {
  path_ = archiver_name;
  std::ifstream file(path_, std::ios::in | std::ios::binary);

  bool check_result = CheckArchiver(file);
  if (!check_result) {
    return;
  }

  // number of parity bits
  uint8_t number_of_parity_bits = GetTwoBytes(file);

  bool compress = GetTwoBytes(file);
  std::map<uint8_t, std::vector<bool>> tree;
  if (compress) {
    ReadTree(file, tree);
  }

  uint8_t ch;
  while(file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
    // filename size
    uint16_t filename_length = GetFilenameSize(ch, file);
    
    // filename
    uint8_t* filename_bytes = new uint8_t[filename_length];
    ReadBytes(file, filename_bytes, filename_length);

    std::string current_filename = "";
    Decode(filename_bytes, filename_length, number_of_parity_bits, current_filename, false);
    delete[] filename_bytes;

    // data size
    size_t data_size = GetDataSize(file, kDataSize);

    // data
    uint8_t* data_bytes = new uint8_t[data_size];
    ReadBytes(file, data_bytes, data_size);

    if (compress) {
      DecodeWithCompress(data_bytes, data_size, number_of_parity_bits, current_filename, true, tree);
    }
    else {
      Decode(data_bytes, data_size, number_of_parity_bits, current_filename, true);
    }
    delete[] data_bytes;
  }

  file.close();
  std::remove(archiver_name.c_str());
}

// create new archive. Write type (HF) and number of parity bits
// Old archive:
// if filename not in names => add file in new archive
// else
//    flag_delete is true => only move cursor
//    else extract file
// delete old archive, rename new
void Archiver::ExtractFiles(const std::string& archiver_name, 
                            const std::vector<std::string>& names, 
                            bool flag_delete) {
  path_ = archiver_name;
  std::ifstream file(path_, std::ios::in | std::ios::binary);

  bool check_result = CheckArchiver(file);
  if (!check_result) {
    return;
  }

  // create new arhciver
  const std::string new_archiver_name = "new.haf";
  WriteType(new_archiver_name, "HF");

  // number of parity bits
  uint8_t number_of_parity_bits = GetTwoBytes(file);
  WriteTwoParts(new_archiver_name, number_of_parity_bits);

  bool compress = GetTwoBytes(file);
  WriteTwoParts(new_archiver_name, compress);
  std::map<uint8_t, std::vector<bool>> tree;
  if (compress) {
    ReadTree(file, tree);
    WriteTree(new_archiver_name, tree);
  }

  int8_t ch;
  while(file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
    // filename size
    uint16_t filename_length = GetFilenameSize(ch, file);
    
    // filename
    uint8_t* filename_bytes = new uint8_t[filename_length];
    ReadBytes(file, filename_bytes, filename_length);

    Reset();
    std::string current_filename = "";
    Decode(filename_bytes, filename_length, number_of_parity_bits, current_filename, false);

    size_t data_size = GetDataSize(file, kDataSize);
    
    // if filename in names
    if (std::find(names.begin(), names.end(), current_filename) != names.end()) {
      if (flag_delete) {
        // skip
        size_t pos = file.tellg();
        file.seekg(pos + data_size);
      }
      else {
        // extract
        uint8_t* data_bytes = new uint8_t[data_size];
        ReadBytes(file, data_bytes, data_size);

        Reset();
        if (compress) {
          DecodeWithCompress(data_bytes, data_size, number_of_parity_bits, current_filename, true, tree);
        }
        else {
          Decode(data_bytes, data_size, number_of_parity_bits, current_filename, true);
        }
        delete[] data_bytes;
      }
    }
    else {
      // copy. write filename length and filename
      for (int i = 0; i < kFilenameLength; i++) {
        WriteTwoParts(new_archiver_name, filename_length >> (i * 8));
      }
      std::ofstream new_archiver(new_archiver_name, std::ios::out | std::ios::app | std::ios::binary);
      for (size_t i = 0; i < filename_length; i++) {
        new_archiver.write(reinterpret_cast<char*>(&filename_bytes[i]), sizeof(filename_bytes[i]));
      }
      new_archiver.close();

      // write data size
      for (int i = 0; i < kDataSize; i++) {
        WriteTwoParts(new_archiver_name, (data_size >> (i * 8)));
      }
  
      // write data bytes
      new_archiver.open(new_archiver_name, std::ios::out | std::ios::app | std::ios::binary);
      for (size_t i = 0; i < data_size; i++) {
        file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        new_archiver.write(reinterpret_cast<char*>(&ch), sizeof(ch));
      }
      new_archiver.close();
    }

    delete[] filename_bytes;
  }
  file.close();
  // delete previous archive; rename new
  const std::string& prev_name = path_;
  std::remove(path_.c_str());
  std::rename(new_archiver_name.c_str(), prev_name.c_str());

  size_t size_after_extract = FileSize(prev_name);
  if (size_after_extract == kObjectType + kNumberOfParityBits) {
    std::remove(prev_name.c_str());
  }
}
