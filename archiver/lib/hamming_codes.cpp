#include "hamming_codes.h"

const uint8_t kByteSize = 8;
const uint8_t kDefaultNumberOfParityBits = 3;

size_t FileSize(const std::string& filename) {
  std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cout << "No such file:( " << filename << "\n";

    return 0;
  }

  size_t last_byte_position = file.tellg();
  file.close();

  return last_byte_position;
}

size_t FileSizeBits(const std::string& filename,
                    const std::map<uint8_t, std::vector<bool>>& tree) {
  std::ifstream file(filename, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    std::cout << "No such file:( " << filename << "\n";

    return 0;
  }
  size_t size = 0;
  uint8_t ch;
  while (file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
    if (tree.find(ch) != tree.end()) {
      size += tree.find(ch)->second.size();
    }
    else {
      std::cout << "No such symbol " << static_cast<int>(ch) << "\n";
    }
  }

  return size;
}

uint64_t CountBytesWithParityBits(uint64_t bits, uint16_t parity_bits) {
  // uint64_t bits = bytes * 8;
  uint64_t block = (1 << parity_bits) - 1;
  uint64_t data_bits = block - parity_bits;
  uint64_t blocks = bits / data_bits;
  if (bits % data_bits != 0) {
    blocks++;
  }
  uint64_t new_bits = blocks * block;
  uint64_t new_bytes = new_bits / 8;
  if (new_bits % 8 != 0) {
    new_bytes++;
  }

  return new_bytes;
}

uint64_t CountBytesWithoutParityBits(uint64_t bytes, uint16_t parity_bits) {
  uint64_t bits = bytes * 8;
  uint64_t block = (1 << parity_bits) - 1;
  uint64_t data_bits = block - parity_bits;
  uint64_t blocks = bits / block;

  uint64_t bits_without_parity = blocks * data_bits;
  uint64_t new_bytes = bits_without_parity / 8;

  if (bits_without_parity % 8 != 0) {
    new_bytes++;
  }

  return new_bytes;
}

void CalculateParityBits(std::vector<bool>& bits, int32_t parity_bits) {
  int32_t block = (1 << parity_bits) - 1;
  for (int i = 0; i < parity_bits; i++) {
    int32_t value = 0;
    int32_t index = (1 << i) - 1;
    while (index < block) {
      for (int j = 0; j < (1 << i); j++) {
        value ^= bits[index];
        index++;
        if (index >= block) {
          break;
        }
      }
      index += (1 << i);
    }
    bits[(1 << i) - 1] = value;
  }
}

bool CheckParityBits(const std::vector<bool>& bits, 
                     const std::vector<bool>& prev_parity_bits, 
                     int32_t count_parity_bits) {
  for (int i = 0; i < count_parity_bits; i++) {
    if (bits[(1 << i) - 1] != prev_parity_bits[i]) {

      return false;
    }
  }

  return true;
}

int32_t FixError(std::vector<bool>& bits, std::vector<bool>& prev_parity_bits, int32_t count_parity_bits) {
  if (CheckParityBits(bits, prev_parity_bits, count_parity_bits)) {
    
    return kNoError;
  }
  int32_t summa = 0;
  for (int i = 0; i < count_parity_bits; i++) {
    if (bits[(1 << i) - 1] != prev_parity_bits[i]) {
      summa += (1 << i);
    }
  }
  // because numeration from 1
  summa--;
  // if summa is 2^i, 
  // then error was in parity bit (i`th)
  if (((summa + 1) & summa) == 0) {

    return kNoError;
  }
  if (summa < bits.size()) {
    bits[summa] = !bits[summa];
    for (int i = 0; i < count_parity_bits; i++) {
      bits[(1 << i) - 1] = 0;
    }
    CalculateParityBits(bits, count_parity_bits);
    if (CheckParityBits(bits, prev_parity_bits, count_parity_bits)) {

      return kErrorFixed;
    }

    return kCannotFix;
  }
  else {

    return kCannotFix;
  }
}

void Archiver::AddByte(int32_t block, int& cnt) {
  for (int i = 0; i < block; i++) {
    if (bits_[i]) {
      byte_ |= (1 << index_byte_);
    }
    index_byte_++;
    if (index_byte_ == (1 << 3)) {
      std::ofstream file(path_, std::ios::app | std::ios::binary);
      file.write(reinterpret_cast<char*>(&byte_), sizeof(byte_));
      file.close();
      cnt++;
      index_byte_ = 0;
      byte_ = 0;
    }
  }
}

bool Archiver::CheckArchiver(std::ifstream& file) {
  // 1 - try to open file
  if (!file.is_open()) {
    std::cout << "Cannot open file\n";

    return false;
  }
  
  // check is it has object type and number of parity bits
  size_t all_file_size = FileSize(path_);
  if (all_file_size < kObjectType + kNumberOfParityBits) {
    std::cout << "Not archiver\n";
    file.close();

    return false;
  }

  // check type
  uint8_t ch;
  bool is_ham_archiver = CheckType(file);

  if (!is_ham_archiver) {
    std::cout << "Not ham arc:(\n";
    file.close();

    return false;
  }

  return true;
}

uint16_t GetFilenameSize(uint8_t ch, std::ifstream& file) {
  uint8_t ch_second;
  file.read(reinterpret_cast<char*>(&ch_second), sizeof(ch_second));
  uint16_t filename_length = MergeTwoParts(ch, ch_second);
  filename_length += (GetTwoBytes(file) << 8);

  return filename_length;
}

size_t GetDataSize(std::ifstream& file, uint8_t size) {
  size_t data_size = 0;
  uint8_t ch;
  for (int i = 0; i < size; i++) {
    ch = GetTwoBytes(file);
    data_size += (ch << (8 * i));
  }

  return data_size;
}

void ReadBytes(std::ifstream& file, uint8_t* arr, size_t size) {
  uint8_t ch;
  for (size_t i = 0; i < size; i++) {
    file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
    arr[i] = ch;
  }
}

void Archiver::AddLast() {
  // last block
  if (index_ != 0) {
    CalculateParityBits(bits_, parity_bits_);
    int cnt = 0;
    AddByte((1 << parity_bits_) - 1, cnt);
  }

  // last byte
  if (index_byte_ != 0) {
    std::ofstream file(path_, std::ios::app | std::ios::binary);
    file.write(reinterpret_cast<char*>(&byte_), sizeof(byte_));
    file.close();
  }

  bits_.clear();
  index_ = 0;
  byte_ = 0;
  index_byte_ = 0;
}

uint8_t DefaultEncoding(const std::vector<bool>& arr) {
  // default encoding (4, 3) - three parity bits
  // using to encode such things as number of parity bits, block length..
  std::vector<bool> data_with_parity_bits(kByteSize);
  data_with_parity_bits[2] = arr[0];
  data_with_parity_bits[4] = arr[1];
  data_with_parity_bits[5] = arr[2];
  data_with_parity_bits[6] = arr[3];
  data_with_parity_bits[0] = arr[0] ^ arr[1] ^ arr[3];
  data_with_parity_bits[1] = arr[0] ^ arr[2] ^ arr[3];
  data_with_parity_bits[3] = arr[1] ^ arr[2] ^ arr[3];

  uint8_t byte = 0;
  for (int i = 0; i <kByteSize - 1; i++) {
    byte |= data_with_parity_bits[i] * (1 << i);
  }

  return byte;
}

uint8_t DefaultDecoding(uint8_t data) {
  // default decoding (4, 3) - three parity bits
  // using to dencode such things as number of parity bits, block length..
  // arr - with parity bits
  std::vector<bool> bits(kByteSize);
  for (int i = 0; i < kByteSize; i++) {
    bits[i] = (data & (1 << i));
  }

  std::vector<bool> prev_parity_bits(kDefaultNumberOfParityBits);
  for (int i = 0; i < kDefaultNumberOfParityBits; i++) {
    prev_parity_bits[i] = bits[(1 << i) - 1];
    bits[(1 << i) - 1] = 0;
  }

  CalculateParityBits(bits, kDefaultNumberOfParityBits);
  int32_t fix_result = FixError(bits, prev_parity_bits, kDefaultNumberOfParityBits);
  if (fix_result == kErrorFixed) {
    std::cout << "error was fixed\n";
  }
  else if (fix_result == kCannotFix) {
    std::cout << "Cannot fix error:(\n";
    return 0;
  }
  
  uint8_t byte = 0;
  int32_t pow = 0;
  for (int i = 0; i < bits.size(); i++) {
    if (((i + 1) & i) == 0) {
      continue;
    }
    byte |= bits[i] * (1 << pow);
    pow += 1;
  }

  return byte;
}

void Archiver::Encode(uint8_t* bytes, int32_t size) {
  int64_t block = (1 << parity_bits_) - 1;
  index_ = 0;
  byte_ = 0;
  index_byte_ = 0;
  bits_.clear();
  bits_.resize(block);
  std::fill(bits_.begin(), bits_.end(), 0);
  
  int cnt = 0;
  uint8_t ch;
  for (int k = 0; k < size; k++) {
    ch = bytes[k];
    // bits:
    for (int j = 0; j < (1 << 3); j++) {
      // check if it is parity bit place
      while (((index_ + 1) & index_) == 0) {
        bits_[index_] = 0;
        index_++;
        // if block is full
        // 1 - calculate parity bits
        // 2 - write data
        if (index_ == block) {
            CalculateParityBits(bits_, parity_bits_);
            AddByte(block, cnt);
            index_ = 0;
            std::fill(bits_.begin(), bits_.end(), 0);
        }
      }
      // current_bit
      bits_[index_] = ch & (1 << j);
      index_++;
      // if block is full
      // 1 - calculate parity bits
      // 2 - write data
      if (index_ == block) {
        CalculateParityBits(bits_, parity_bits_);
        AddByte(block, cnt);
        index_ = 0;
        std::fill(bits_.begin(), bits_.end(), 0);
      }
    }
  }
}

void Archiver::EncodeWithCompress(uint8_t* bytes, int32_t size, 
                                  const std::map<uint8_t, std::vector<bool>>& tree) {
  int64_t block = (1 << parity_bits_) - 1;

  index_ = 0;
  byte_ = 0;
  index_byte_ = 0;
  bits_.clear();
  bits_.resize(block);
  std::fill(bits_.begin(), bits_.end(), 0);


  int cnt = 0;

  uint8_t ch;
  for (int k = 0; k < size; k++) {
    ch = bytes[k];
    std::vector<bool> new_bits;
    if (tree.find(ch) != tree.end()) {
      new_bits = tree.find(ch)->second;
    }
    else {
      std::cout << "Encode: No such symbol " << static_cast<int>(ch) << "\n";
    }
    // bits:
    for (const auto& bit: new_bits) {
      // check if it is parity bit place
      while (((index_ + 1) & index_) == 0) {
        bits_[index_] = 0;
        index_++;
        // if block is full
        // 1 - calculate parity bits
        // 2 - write data
        if (index_ == block) {
            CalculateParityBits(bits_, parity_bits_);
            AddByte(block, cnt);
            index_ = 0;
            std::fill(bits_.begin(), bits_.end(), 0);
        }
      }
      // current_bit
      bits_[index_] = bit;
      index_++;
      // if block is full
      // 1 - calculate parity bits
      // 2 - write data
      if (index_ == block) {
        CalculateParityBits(bits_, parity_bits_);
        AddByte(block, cnt);
        index_ = 0;
        std::fill(bits_.begin(), bits_.end(), 0);
      }
    }
  }
}

void Archiver::Decode(uint8_t* bytes, int32_t size, uint8_t count_of_parity_bits, 
                      std::string& filename, bool flag_write) {
  int64_t block = (1 << count_of_parity_bits) - 1;

  int32_t index_prev_parity_bits = 0;
  std::vector<bool> prev_parity_bits(count_of_parity_bits);

  bits_.clear();
  byte_ = 0;
  index_byte_ = 0;
  index_ = 0;
  bits_.resize((1 << count_of_parity_bits) - 1);
  std::fill(bits_.begin(), bits_.end(), 0);

  uint8_t ch;

  std::ifstream file(path_, std::ios::in | std::ios::binary);
  for (int i = 0; i < size; i++) {
    ch = bytes[i];
    // bits
    for (int j = 0; j < (1 << 3); j++) {
      bits_[index_] = ch & (1 << j);
      index_++;
      // if block is full
      if (index_ == block) {
        // remember previous parity bits
        for (int i = 0; i < count_of_parity_bits; i++) {
          prev_parity_bits[i] = bits_[(1 << i) - 1];
          bits_[(1 << i) - 1] = 0;
        }
        // new parity bits
        CalculateParityBits(bits_, count_of_parity_bits);
  
        int32_t fix_result = FixError(bits_, prev_parity_bits, count_of_parity_bits);
        if (fix_result == kErrorFixed) {
          std::cout << "error was fixed\n";
        }
        else if (fix_result == kCannotFix) {
          std::cout << "Cannot fix error:(\n";
          return;
        }
        // read 8 bit => write byte
        for (int i = 0; i < block; i++) {
          // skip parity bits
          if (((i + 1) & i) == 0) {
            continue;
          }
          if (bits_[i]) {
            byte_ |= (1 << index_byte_);
          }
          index_byte_++;
          if (index_byte_ == (1 << 3)) {
            if (flag_write) {
              std::ofstream file(filename, std::ios::app | std::ios::binary);
              file.write(reinterpret_cast<char*>(&byte_), sizeof(byte_));
              file.close();
            }
            else {
              filename += byte_;
            }
            byte_ = 0;
            index_byte_ = 0;
          }
        }
        index_ = 0;
        std::fill(bits_.begin(), bits_.end(), 0);
      }
    }
  } 
}

void Archiver::DecodeWithCompress(uint8_t* bytes, int32_t size, uint8_t count_of_parity_bits,
                          std::string& filename, bool flag_write, 
                          const std::map<uint8_t, std::vector<bool>>& tree) {
  std::map<std::vector<bool>, uint8_t> tree_reversed;
  for (const auto& [symb, new_bits]: tree) {
    tree_reversed[new_bits] = symb;
  }
  int64_t block = (1 << count_of_parity_bits) - 1;

  int32_t index_prev_parity_bits = 0;
  std::vector<bool> prev_parity_bits(count_of_parity_bits);
  std::vector<bool> current_bits;

  bits_.clear();
  index_ = 0;
  bits_.resize((1 << count_of_parity_bits) - 1);
  std::fill(bits_.begin(), bits_.end(), 0);
  

  uint8_t ch;

  std::ifstream file(path_, std::ios::in | std::ios::binary);
  for (int i = 0; i < size; i++) {
    ch = bytes[i];
    // bits
    for (int j = 0; j < (1 << 3); j++) {
      bits_[index_] = ch & (1 << j);
      index_++;
      // if block is full
      if (index_ == block) {
        // remember previous parity bits
        for (int i = 0; i < count_of_parity_bits; i++) {
          prev_parity_bits[i] = bits_[(1 << i) - 1];
          bits_[(1 << i) - 1] = 0;
        }
        // new parity bits
        CalculateParityBits(bits_, count_of_parity_bits);
  
        int32_t fix_result = FixError(bits_, prev_parity_bits, count_of_parity_bits);
        if (fix_result == kErrorFixed) {
          std::cout << "error was fixed\n";
        }
        else if (fix_result == kCannotFix) {
          std::cout << "Cannot fix error:(\n";
          return;
        }
      
        // read 8 bit => write byte
        for (int i = 0; i < block; i++) {
          // skip parity bits
          if (((i + 1) & i) == 0) {
            continue;
          }
          current_bits.push_back(bits_[i]);
          if (tree_reversed.count(current_bits)) {
            uint8_t symb = tree_reversed[current_bits];
            if (flag_write) {
              std::ofstream file(filename, std::ios::app | std::ios::binary);
              file.write(reinterpret_cast<char*>(&symb), sizeof(symb));
              file.close();
            }
            else {
              filename += symb;
            }
            current_bits.clear();
          }
        }
        index_ = 0;
        std::fill(bits_.begin(), bits_.end(), 0);
      }
    }
  } 
}

uint8_t MergeTwoParts(uint8_t first, uint8_t second) {
  uint8_t first_part = DefaultDecoding(first);
  uint8_t second_part = DefaultDecoding(second);

  return (second_part << 4) | first_part;
}

uint8_t GetTwoBytes(std::ifstream& file) {
  uint8_t ch;
  file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
  uint8_t first_symb = ch;
  file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
  uint8_t second_symb = ch;
  uint8_t result = MergeTwoParts(first_symb, second_symb);

  return result;
}

void WriteTwoParts(const std::string& filename, uint8_t symbol) {
  std::ofstream file(filename, std::ios::app | std::ios::binary);
  if (!file.is_open()) {
    std::cout << "cannot open file\n";

    return;
  }
  std::vector<bool> bits(4);
  for (int i = 0; i < 4; i++) {
    bits[i] = symbol & (1 << i);
  }
  uint8_t byte = DefaultEncoding(bits);
  file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
  for (int i = 4; i < 8; i++) {
    bits[i - 4] = symbol & (1 << i);
  }
  byte = DefaultEncoding(bits);
  file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
  file.close();
}

void WriteType(const std::string& filename, const std::string& type) {
  for (int i = 0; i < type.length(); i++) {
    WriteTwoParts(filename, type[i]);
  }
}

bool CheckType(std::ifstream& file) {
  uint8_t first_symbol = GetTwoBytes(file);
  uint8_t second_symbol = GetTwoBytes(file);

  return first_symbol == 'H' && second_symbol == 'F';
}

void ReadTree(std::ifstream& file, std::map<uint8_t, std::vector<bool>>& tree) {
  uint32_t tree_size = GetDataSize(file, 4);
  for (uint32_t i = 0; i < tree_size; i++) {
    uint8_t symb = GetTwoBytes(file);
    uint8_t len = GetTwoBytes(file);
    uint8_t byte_count = len / 8;
    if (len % 8 != 0) {
        byte_count++;
    }
    std::vector<bool> bits;
    for (int i = 0; i < byte_count; i++) {
      uint8_t byte = GetTwoBytes(file);
      for (int j = 0; j < (1 << 3); j++) {
        if (bits.size() >= len) {
            break;
        }
        if (byte & (1 << j)) {
          bits.push_back(1);
        }
        else {
          bits.push_back(0);
        }
      }
    }
    tree[symb] = bits;
  }
}

void WriteTree(const std::string& filename, std::map<uint8_t, std::vector<bool>>& tree) {
  uint32_t tree_size = tree.size();

  for (int i = 0; i < 4; i++) {
    WriteTwoParts(filename, tree_size >> (i * 8));
  }
  for (const auto& [symb, bits]: tree) {
    WriteTwoParts(filename, symb);
    WriteTwoParts(filename, bits.size());
    uint8_t byte = 0;
    uint8_t index_bit = 0;
    for (const auto& bit: bits) {
      byte |= bit * (1 << index_bit);
      index_bit++;
      if (index_bit == (1 << 3)) {
        WriteTwoParts(filename, byte);
        byte = 0;
        index_bit = 0;
      }
    }
    if (index_bit) {
      WriteTwoParts(filename, byte);
    }
  }
}