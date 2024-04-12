#include "bmp_writer.h"
#include <fstream>

const char* kCannotOpenFolder = "Cannot open directory to read data";

RGBPixel::RGBPixel(unsigned char r, unsigned char g,
           unsigned char b, unsigned char alpha) {
  color = alpha;
  color = (color << 8) + r;
  color = (color << 8) + g;
  color = (color << 8) + b;
}

Image::Image(int32_t w, int32_t h) {
  width = w;
  height = h;
}

void Image::WriteHeader(std::ostream& output) {
  unsigned char header[kHeaderSize]{};
  uint32_t file_size = kHeaderSize + kDIBHeaderSize + kColorTableSize + (width + 1) / 2 * height; 

  // identify BMP
  header[0] = 'B';
  header[1] = 'M';
  // size of the BMP file in bytes
  header[2] = file_size;
  header[3] = file_size >> 8;
  header[4] = file_size >> 16;
  header[5] = file_size >> 24;
  // Reserved x2
  header[6] = 0;
  header[7] = 0;
  header[8] = 0;
  header[9] = 0;
  // offset
  header[10] = kHeaderSize + kDIBHeaderSize + kColorTableSize;
  header[11] = 0;
  header[12] = 0;
  header[13] = 0;

  output.write(reinterpret_cast<char*>(header), kHeaderSize);
}

void Image::WriteInformation(std::ostream& output) {
  unsigned char information[kDIBHeaderSize];
  
  // size of header
  information[0] = kDIBHeaderSize;
  information[1] = 0;
  information[2] = 0;
  information[3] = 0;
  // width
  information[4] = width;
  information[5] = width >> 8;
  information[6] = width >> 16;
  information[7] = width >> 24;
  // height
  information[8] = height;
  information[9] = height >> 8;
  information[10] = height >> 16;
  information[11] = height >> 24;
  // color planes
  information[12] = 1;
  information[13] = 0;
  // bits per pixel
  information[14] = 4;
  information[15] = 0;
  // comprassion
  information[16] = 0;
  information[17] = 0;
  information[18] = 0;
  information[19] = 0;
  // image size
  information[20] = 0;
  information[21] = 0;
  information[22] = 0;
  information[23] = 0;
  // horizontal resolution (none)
  information[24] = 0;
  information[25] = 1;
  information[26] = 0;
  information[27] = 0;
  // vertical resolution (none)
  information[28] = 0;
  information[29] = 1;
  information[30] = 0;
  information[31] = 0;
  // number of colors in the color palette
  information[32] = 5;
  information[33] = 0;
  information[34] = 0;
  information[35] = 0;
  // number of important colors used
  information[36] = 5;
  information[37] = 0;
  information[38] = 0;
  information[39] = 0;

  output.write(reinterpret_cast<char*>(information), kDIBHeaderSize);
}

void Image::WriteColorTable(std::ostream& output) {
  ColorTable table;
  table.colors[0] = RGBPixel(255, 255, 255, 0);
  table.colors[1] = RGBPixel(0, 255, 81, 0);
  table.colors[2] = RGBPixel(255, 247, 0, 0);
  table.colors[3] = RGBPixel(148, 0, 141, 0);
  table.colors[4] = RGBPixel(0, 0, 0, 0);

  char color_table[ColorTable::kSize * RGBPixel::kSize]{};
  for (int i = 0; i < ColorTable::kSize * RGBPixel::kSize; i += RGBPixel::kSize) {
    color_table[i] = table.colors[i / RGBPixel::kSize].color & 0xFF;
    color_table[i + 1] = table.colors[i / RGBPixel::kSize].color >> 8 & 0xFF;
    color_table[i + 2] = table.colors[i / RGBPixel::kSize].color >> 16 & 0xFF;
    color_table[i + 3] = table.colors[i / RGBPixel::kSize].color >> 24;
  }

  output.write(reinterpret_cast<char*>(color_table), ColorTable::kSize * RGBPixel::kSize);
}

void Image::WriteBmp(Pixel** pixels, const char* path) {
  std::ofstream output(path, std::ios_base::out | std::ios_base::binary);
  if (!output) {
    std::cout << path << " " << kCannotOpenFolder;
    exit(EXIT_SUCCESS);
  }

  WriteHeader(output);
  WriteInformation(output);
  WriteColorTable(output);
  
  int32_t buffer_size = ((((width + 1) / 2) + 3) / 4) * 4;
  unsigned char* row = new unsigned char[buffer_size];
  for (int i = 0; i < height; i++) {
    int32_t ind = 0;
    for (int j = 0; j < width - 1; j += 2) {
      row[ind] = static_cast<unsigned char>((pixels[i][j].color << 4) + pixels[i][j + 1].color);
      ++ind;
    }
    if (ind < buffer_size && width % 2 == 1) {
      row[ind] = static_cast<unsigned char>(pixels[i][width - 1].color << 4);
    }
    output.write(reinterpret_cast<char*>(row), buffer_size);
  }
  delete[] row;
}
