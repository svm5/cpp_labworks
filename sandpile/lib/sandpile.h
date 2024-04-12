#pragma once
#include <iostream>
#include "my_queue.cpp"

struct Sandpile {
  static const int32_t INF = INT32_MAX;
  static const int32_t MIN_INF = INT32_MIN;

  int32_t mn_x = INF;
  int32_t mx_x = MIN_INF;
  int32_t mn_y = INF;
  int32_t mx_y = MIN_INF;

  int32_t size_x = 0;
  int32_t size_y = 0;

  int32_t increase_x = 0;
  int32_t increase_y = 0;

  uint64_t** array;

  Queue queue;

  ~Sandpile();

  void CreateSandpile(const char* filename);
  void FindCorners(const char* filename);
  void FillArray(const char* filename);
  bool CheckExpand(int32_t row, int32_t col);
  void ExpandWidth();
  void ExpandHeight();
  void FillQueueUnstableCells();
  void Scatter(int32_t row, int32_t col);
  void UpdateQueue(int32_t row, int32_t col);
  void UpdateCorners(int32_t row, int32_t col);
  void Spilling(const char* path, int64_t mx, int64_t frequency);
  void Draw(const char* path);
  void Clear();
};
