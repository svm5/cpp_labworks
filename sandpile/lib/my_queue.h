#pragma once
#include <iostream>

class Node {
public:
  int32_t i;
  int32_t j;
  Node* next;

  Node (int32_t value_i, int32_t value_j);
};

class Queue {
public:
  ~Queue();

  void Push(int32_t i, int32_t j);
  void Pop();
  void Clear();
  bool IsEmpty() const;
  std::pair<int32_t, int32_t> Front() const;

private:
  Node* head = nullptr;
  Node* tail = nullptr;
};
