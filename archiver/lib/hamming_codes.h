#include "archiver.h"

const int32_t kNoError = 0;
const int32_t kErrorFixed = 1;
const int32_t kCannotFix = -1;


size_t FileSize(const std::string& filename);
size_t FileSizeBits(const std::string& filename,
                    const std::map<uint8_t, std::vector<bool>>& tree);

uint64_t CountBytesWithParityBits(uint64_t bytes, uint16_t parity_bits);
uint64_t CountBytesWithoutParityBits(uint64_t bytes, uint16_t parity_bits);

void CalculateParityBits(std::vector<bool>& bits, int32_t parity_bits);

bool CheckParityBits(const std::vector<bool>& bits, const std::vector<bool>& prev_parity_bits, int32_t count_parity_bits);

int32_t FixError(std::vector<bool>& bits, std::vector<bool>& prev_parity_bits, int32_t count_parity_bits);  // !!!

uint8_t DefaultEncoding(const std::vector<bool>& arr);

uint8_t DefaultDecoding(uint8_t data);

uint16_t GetFilenameSize(uint8_t ch, std::ifstream& file);
size_t GetDataSize(std::ifstream& file, uint8_t size);

void ReadBytes(std::ifstream& file, uint8_t* arr, size_t size);

uint8_t MergeTwoParts(uint8_t first, uint8_t second);

uint8_t GetTwoBytes(std::ifstream& file);

void WriteTwoParts(const std::string& filename, uint8_t symbol);

void WriteType(const std::string& filename, const std::string& type);

bool CheckType(std::ifstream& file);

void ReadTree(std::ifstream& file, std::map<uint8_t, std::vector<bool>>& tree);
void WriteTree(const std::string& filename, std::map<uint8_t, std::vector<bool>>& tree);