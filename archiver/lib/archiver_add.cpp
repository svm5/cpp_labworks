#include "hamming_codes.h"

bool CheckFileExists(const std::string& filename) {
  std::ifstream file(filename, std::ios::in | std::ios::binary);
  if (!file.is_open()) {

    return false;
  } 
  file.close();

  return true;
}

uint8_t Archiver::GetNumberOfParityBits(const std::string& archiver_name) {
  std::ifstream file(archiver_name, std::ios::in | std::ios::binary);
  if (!file.is_open()) {

    return -1;
  } 
  file.seekg(kObjectType);

  uint8_t parity = GetTwoBytes(file);
  file.close();

  return parity;
}

void Archiver::AddFile(const std::string& archiver_name, 
                       uint8_t number_of_parity_bits, 
                       const std::string& filename) {
  path_ = archiver_name;
  parity_bits_ = number_of_parity_bits;
  bits_.resize((1 << parity_bits_) - 1);
  // encode filename
  if (!CheckFileExists(filename)) {
    return;
  }
  
  std::ifstream file(archiver_name, std::ios::in | std::ios::binary);
  file.seekg(kObjectType + kNumberOfParityBits); // +2 - compress or not

  bool compress = GetTwoBytes(file);

  std::map<uint8_t, std::vector<bool>> tree;
  if (compress) {
    ReadTree(file, tree);
  }
  file.close();

  uint64_t filename_length = filename.length();
  uint64_t size_filename = CountBytesWithParityBits(filename_length * 8, parity_bits_);

  // write filename size
  for (int i = 0; i < kFilenameLength; i++) {
    WriteTwoParts(path_, size_filename >> (i * 8));
  }

  // encode filename
  uint8_t* filename_bytes = new uint8_t[filename_length];
  for (int32_t i = 0; i < filename_length; i++) {
    filename_bytes[i] = filename[i];
  }

  Encode(filename_bytes, filename_length);
  AddLast();
  delete[] filename_bytes;

  // encode data size
  size_t data_size = FileSize(filename);
  size_t data_size_with_parity_bits;
  if (compress) {
    size_t data_size_bits = FileSizeBits(filename, tree);
    data_size_with_parity_bits = CountBytesWithParityBits(data_size_bits, parity_bits_);
  }
  else {
    
    data_size_with_parity_bits = CountBytesWithParityBits(data_size * 8, parity_bits_);
  }
  for (int i = 0; i < kDataSize; i++) {
    WriteTwoParts(path_, (data_size_with_parity_bits >> (i * 8)));
  }

  // encode data
  std::ifstream input(filename, std::ios::in | std::ios::binary);
  uint8_t* input_bytes = new uint8_t[data_size];
  uint8_t ch;
  for (size_t i = 0; i < data_size; i++) {
    input.read(reinterpret_cast<char*>(&ch), sizeof(ch));
    input_bytes[i] = ch;
  }
  input.close();

  if (compress) {
    EncodeWithCompress(input_bytes, data_size, tree);
  }
  else {
    Encode(input_bytes, data_size);
  }
  AddLast();
  
  delete[] input_bytes;
}
