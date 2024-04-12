#include "archiver_add.cpp"
#include "heap.h"

void Count(const std::vector<std::string>& filenames, std::map<uint8_t, size_t>& mem) {
  for (const auto& filename: filenames) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      return;
    }

    uint8_t ch;
    while(file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
      if (mem.count(ch)) {
        mem[ch]++;
      }
      else {
        mem[ch] = 1;
      }
    }
  }
}

void BuildTree(const std::map<uint8_t, size_t>& mem,
               std::map<uint8_t, std::vector<bool>>& new_bits) {
  Heap heap;
  for (const auto& [symb, cnt]: mem) {
    // std::cout << static_cast<int>(symb) << " " << cnt << '\n';
    heap.arr.push_back(new Node(symb, cnt));
  }
  while (heap.arr.size() > 1) {
    Node* mn1 = heap.ExtractMin();
    Node* mn2 = heap.ExtractMin();

    Node* new_node = new Node(' ', mn1->summa + mn2->summa);
    new_node->left = mn1;
    new_node->right = mn2;

    heap.Insert(new_node);
  }

  std::vector<bool> path;
  Preordered(heap.arr[0], path, new_bits);
}

void Archiver::Create(const std::string& archiver_name, 
                      uint8_t number_of_parity_bits,
                      const std::vector<std::string>& filenames,
                      bool compress) {
  uint32_t cnt_good = 0;
  for (uint32_t i = 0; i < filenames.size(); i++) {
    cnt_good += CheckFileExists(filenames[i]);
  }
  if (cnt_good < 2) {
    std::cout << "You need 2 and more files (which exist) to create archive\n";
    return;
  }

  path_ = archiver_name;
  parity_bits_ = number_of_parity_bits;
  bits_.resize((1 << parity_bits_) - 1);
  // write HF - object type
  WriteType(archiver_name, kType);

  // write number parity bits
  WriteTwoParts(archiver_name, parity_bits_);

  // // do we need to compress
  WriteTwoParts(archiver_name, compress);

  // write tree
  if (compress) {
    std::map<uint8_t, size_t> mem;
    Count(filenames, mem);
    std::map<uint8_t, std::vector<bool>> new_bits;
    BuildTree(mem, new_bits);
    WriteTree(archiver_name, new_bits);
  }

  for (const auto& name: filenames) {
    AddFile(archiver_name, number_of_parity_bits, name);
  }
}
