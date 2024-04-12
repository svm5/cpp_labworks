#pragma once
#include <iostream>

struct Pixel {
  static const unsigned char kWhite = 0;
  static const unsigned char kGreen = 1;
  static const unsigned char kYellow = 2;
  static const unsigned char kPurple = 3;
  static const unsigned char kBlack = 4;

  unsigned char color;
};

struct RGBPixel {
  static const int16_t kSize = 4;

  uint32_t color;

  RGBPixel() = default;
  RGBPixel(unsigned char r, unsigned char g,
           unsigned char b, unsigned char alpha);
};

struct ColorTable {
  static const int16_t kSize = 5;

  RGBPixel colors[kSize];

  ColorTable() = default;
};

struct Image {
  static const int16_t kHeaderSize = 14;
  static const int16_t kDIBHeaderSize = 40;
  static const int16_t kColorTableSize = 20;

  int32_t width;
  int32_t height;

  Image() = default;
  Image(int32_t w, int32_t h);

  void WriteBmp(Pixel** pixels, const char* path);
  void WriteHeader(std::ostream& output);
  void WriteInformation(std::ostream& output);
  void WriteColorTable(std::ostream& output);
};
