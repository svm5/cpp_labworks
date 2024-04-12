#include "sandpile.h"
#include "bmp_writer.cpp"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

const int32_t kUnstableCell = 4;
const int32_t kBufferSize = 1000;
const int32_t kWithoutIndex = -1;
const int16_t kFilenameBufferSize = 20;

char* CreateFilename(const char* path, int32_t ind) {
  char* filename{new char[kBufferSize]{}};
  strcpy(filename, path);
  strcat(filename, "result");
  if (ind != kWithoutIndex) {
    strcat(filename, "_");
    char buffer[kFilenameBufferSize];
    sprintf(buffer, "%d", ind);
    strcat(filename, buffer);
  }
  strcat(filename, ".bmp");
  return filename;
}

Sandpile::~Sandpile() {
  for (int i = 0; i < size_y; i++) {
    delete array[i];
  }
  delete array;
}

void Sandpile::FindCorners(const char* filename) {
  std::ifstream file(filename);
  std::string line;
  char delimiter = '\t';
  int32_t x, y, grains;
  while (file >> x >> y >> grains) {
    mx_x = std::max(mx_x, x);
    mn_x = std::min(mn_x, x);
    mx_y = std::max(mx_y, y);
    mn_y = std::min(mn_y, y);
  }
}

void Sandpile::FillArray(const char* filename) {
  std::ifstream file_fill_array(filename);
  std::string line;
  char delimiter = '\t';
  int32_t x, y, grains;
  while (file_fill_array >> x >> y >> grains) {
    array[increase_y + y - mn_y][increase_x + x - mn_x] = grains;
  }
}

void Sandpile::CreateSandpile(const char* filename) {
  FindCorners(filename);

  increase_x = (mx_x - mn_x + 2) / 2;
  increase_y = (mx_y - mn_y + 2) / 2;
  size_x = 2 * increase_x + (mx_x - mn_x + 1);
  size_y = 2 * increase_y + (mx_y - mn_y + 1);

  array = new uint64_t*[size_y];
  for (int i = 0; i < size_y; i++) {
    array[i] = new uint64_t[size_x]{};
  }

  FillArray(filename);
}

void Sandpile::ExpandWidth() {
  uint64_t** new_arr = new uint64_t*[size_y];
  for (int i = 0; i < size_y; i++) {
    new_arr[i] = new uint64_t[2 * size_x]{};
    for (int j = 0; j < size_x; j++) {
      new_arr[i][size_x / 2 + j] = array[i][j];
    }
  }
  for (int i = 0; i < size_y; i++) {
    delete array[i];
  }
  delete array;
  array = new_arr;
  size_x *= 2;
}

void Sandpile::ExpandHeight() {
  uint64_t** new_arr = new uint64_t*[2 * size_y];
  for (int i = 0; i < 2 * size_y; i++) {
    new_arr[i] = new uint64_t[size_x]{};
  }
  for (int i = 0; i < size_y; i++) {
    for (int j = 0; j < size_x; j++) {
      new_arr[size_y / 2 + i][j] = array[i][j];
    }
  }
  for (int i = 0; i < size_y; i++) {
    delete array[i];
  }
  delete array;
  array = new_arr;
  size_y *= 2;
}

void Sandpile::Clear() {
  queue.Clear();
  mn_x = INF;
  mx_x = MIN_INF;
  mn_y = INF;
  mx_y = MIN_INF;
}

void Sandpile::FillQueueUnstableCells() {
  // !!! queue.Clear() И обнуление mx mn вынесено в отжельную функцию Clear()
  for (int i = 0; i < size_y; i++) {
    for (int j = 0; j < size_x; j++) {
      if (array[i][j] >= kUnstableCell) {
        queue.Push(i, j);
      }
    }
  }
}

bool Sandpile::CheckExpand(int32_t row, int32_t col) {
  if (row == 0 || row == size_y - 1) {
    ExpandHeight();
    Clear();  // !!!
    FillQueueUnstableCells();
    return true;
  }
  if (col == 0 || col == size_x - 1) {
    ExpandWidth();
    Clear();  // !!!
    FillQueueUnstableCells();
    return true;
  }
  return false;
}

void Sandpile::Scatter(int32_t row, int32_t col) {
  while (array[row][col] >= kUnstableCell) {
    array[row][col] -= kUnstableCell;
    ++array[row - 1][col];
    ++array[row][col - 1];
    ++array[row + 1][col];
    ++array[row][col + 1];
  }
}

void Sandpile::UpdateQueue(int32_t row, int32_t col) {
  if (array[row - 1][col] >= kUnstableCell) {
    queue.Push(row - 1, col);
  }
  if (array[row][col - 1] >= kUnstableCell) {
    queue.Push(row, col - 1);
  }
  if (array[row + 1][col] >= kUnstableCell) {
    queue.Push(row + 1, col);
  }
  if (array[row][col + 1] >= kUnstableCell) {
    queue.Push(row, col + 1);
  }
}

void Sandpile::UpdateCorners(int32_t row, int32_t col) {
  if (array[row][col - 1]) {
    mn_x = std::min(mn_x, col - 1);
  }
  if (array[row][col + 1]) {
    mx_x = std::max(mx_x, col + 1);
  }
  if (array[row - 1][col]) {
    mn_y = std::min(mn_y, row - 1);
  }
  if (array[row + 1][col]) {
    mx_y = std::max(mx_y, row + 1);
  }
}

void Sandpile::Spilling(const char* path, int64_t mx, int64_t frequency) {
  FillQueueUnstableCells();
  int64_t count = 0;
  int32_t count_saving = 0;
  while (!queue.IsEmpty()) {
    std::pair<int32_t, int32_t> point = queue.Front();
    int32_t row = point.first;
    int32_t col = point.second;
    queue.Pop();
    if (array[row][col] < kUnstableCell) {
      continue;
    }
    bool check_expand = CheckExpand(row, col);
    if (check_expand) {
      continue;
    }

    Scatter(row, col);
    UpdateQueue(row, col);
    UpdateCorners(row, col);

    ++count;
    if (frequency > 0 && count % frequency == 0) {
      char* filename = CreateFilename(path, count_saving);
      Draw(filename);
      filename = nullptr;
      delete[] filename;
      ++count_saving;
    }
    if (count == mx) {
      return;
    }
  }
}

void Sandpile::Draw(const char* path) {
  int32_t width = mx_x - mn_x + 1;
  int32_t height = mx_y - mn_y + 1;

  Pixel** pixels = new Pixel*[height];

  for (int i = 0; i < height; i++) {
    pixels[i] = new Pixel[width];
    for (int j = 0; j < width; j++) {
      uint64_t current = array[mn_y + i][mn_x + j];
      if (current == 0) {
        pixels[i][j].color = Pixel::kWhite;
      }
      else if (current == 1) {
        pixels[i][j].color = Pixel::kGreen;
      }
      else if (current == 2) {
        pixels[i][j].color = Pixel::kYellow;
      }
      else if (current == 3) {
        pixels[i][j].color = Pixel::kPurple;
      }
      else {
        pixels[i][j].color = Pixel::kBlack;
      }
    }
  }
  Image img(width, height);
  img.WriteBmp(pixels, path);

  for (int i = 0; i < height; i++) {
    delete[] pixels[i];
  }
  delete[] pixels;
}
