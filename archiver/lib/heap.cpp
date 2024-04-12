#include "heap.h"

Node::Node(uint8_t val, size_t sum) {
    value = val;
    summa = sum;
    left = nullptr;
    right = nullptr;
}

void Heap::SiftUp(int i) {
    while (i > 0 && arr[i]->summa < arr[(i - 1) / 2]->summa) {
      Node* temp = arr[i];
      arr[i] = arr[(i - 1) / 2];
      arr[(i - 1) / 2] = temp;
      i = (i - 1) / 2;
    }
}

void Heap::Insert(Node* node) {
    arr.push_back(node);
    SiftUp(arr.size() - 1);
}

Node* Heap::ExtractMin() {
    Node* temp = arr[0];
    arr[0] = arr[arr.size() - 1];
    arr.pop_back();
    SiftDown(0);
    
    return temp;
}

void Heap::SiftDown(int32_t i) {
    while (2 * i + 1 < arr.size()) {
      int left_ind = 2 * i + 1;
      int right_ind = 2 * i + 2;
      int mn_ind = i;

      if (arr[left_ind]->summa < arr[mn_ind]->summa) {
        mn_ind = left_ind;
      }
      if (right_ind < arr.size() && arr[right_ind]->summa < arr[mn_ind]->summa) {
        mn_ind = right_ind;
      }
      if (mn_ind == i) {
        break;
      }
      
      std::swap(arr[i], arr[mn_ind]);
      i = mn_ind;
    }
}

void Heap::Build() {
    for (int i = arr.size() / 2; i >= 0; i--) {
      SiftDown(i);
    }
}

void Preordered(Node* root, std::vector<bool>& path, std::map<uint8_t, std::vector<bool>>& mem) {
  if (root) {
    if (root->left) {
      path.push_back(0);
      Preordered(root->left, path, mem);
      path.pop_back();
      path.push_back(1);
      Preordered(root->right, path, mem);
      path.pop_back();
    }
    else {
      mem[root->value] = path;
    }
  }
}