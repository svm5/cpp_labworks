#include "archiver.h"

Archiver::Archiver(uint16_t parity_bits, const std::string& path)
  : parity_bits_(parity_bits)
  , path_(path)
  {
    bits_.resize((1 << parity_bits_) - 1);
  }

Archiver::Archiver(const std::string& path)
  : path_(path)
  {}

void Archiver::Reset() {
  byte_ = 0;
  index_byte_ = 0;
  index_ = 0;
}