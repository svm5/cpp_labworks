#pragma once

#include <iostream>
#include <vector>
#include <map>

struct Node {
  uint8_t value;
  size_t summa;
  Node* left;
  Node* right;

  Node(uint8_t val, size_t sum);
};

struct Heap {
  std::vector<Node*> arr;

  void SiftUp(int i);

  void Insert(Node* node);

  Node* ExtractMin();

  void SiftDown(int32_t i);

  void Build();
};

void Preordered(Node* root, std::vector<bool>& path, std::map<uint8_t, std::vector<bool>>& mem);