#include "my_queue.h"

Node::Node(int32_t value_i, int32_t value_j) {
  i = value_i;
  j = value_j;
  next = nullptr;
}

Queue::~Queue() {
  while (head != nullptr) {
    Node* current = head;
    head = head->next;
    delete current;
  }
  delete head;
}

void Queue::Push(int32_t i, int32_t j) {
  Node* temp = new Node(i, j);
  if (head == nullptr) {
    tail = temp;
    head = tail;
    return;
  }
  Node* cop = tail;
  cop->next = temp;
  tail = temp;
}

void Queue::Pop() {
  Node* current = head;
  head = head->next;
  delete current;
}

void Queue::Clear() {
  while (!IsEmpty()) {
    Pop();
  }
}

bool Queue::IsEmpty() const {
  return head == nullptr;
}

std::pair<int32_t, int32_t> Queue::Front() const {
  return std::make_pair(head->i, head->j);
}
