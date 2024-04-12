#include <lib/hamming_codes.h>
#include <gtest/gtest.h>
#include <sstream>
#include <ctime>
#include <fstream>

bool CompareFiles(const std::string& first, const std::string& second) {
  std::ifstream file1(first, std::ios::in | std::ios::binary | std::ios::ate);
  std::ifstream file2(second, std::ios::in | std::ios::binary | std::ios::ate);

  if (file1.tellg() < file2.tellg()) {
    return false;
  }

  file1.seekg(0, std::ios::beg);
  file2.seekg(0, std::ios::beg);

  uint8_t ch1;
  uint8_t ch2;
  while (file2.read(reinterpret_cast<char*>(&ch2), sizeof(ch2))) {
    file1.read(reinterpret_cast<char*>(&ch1), sizeof(ch1));

    if (ch1 != ch2) {

      return false;
    }
  }

  return true;
}

// hamming tests
TEST(HammingCodesTestSuite, CountBytesWithParityBitsTest) {
  std::vector<uint64_t> bytes_count = {40, 1, 98};
  std::vector<uint8_t> parity_bits = {2, 3, 5};

  std::vector<uint64_t> results = {120, 70, 51, 3, 2, 4, 294, 172, 121};

  for (int i = 0; i < bytes_count.size(); i++) {
    for (int j = 0; j < parity_bits.size(); j++) {
      uint64_t answer = CountBytesWithParityBits(bytes_count[i] * 8, parity_bits[j]);
      ASSERT_EQ(answer, results[i * parity_bits.size() + j]);
    }
  } 
}

TEST(HammingCodesTestSuite, CountBytesWithoutParityBitsTest) {
  std::vector<uint64_t> bytes_count = {120, 70, 51, 3, 2, 4, 294, 172, 121};
  std::vector<uint8_t> parity_bits = {2, 3, 5};

  std::vector<uint64_t> results = {40, 40, 40, 1, 1, 1, 98, 98, 98};

  for (int i = 0; i < bytes_count.size(); i++) {
    uint64_t answer = CountBytesWithoutParityBits(bytes_count[i], parity_bits[i % parity_bits.size()]);
    uint64_t delta = ((1 << parity_bits[i % parity_bits.size()]) - 1) / 8 + 1;
    ASSERT_TRUE(answer <= results[i] + delta);
  } 
}

TEST(HammingCodesTestSuite, CalculateParityBitsTest) {
  std::vector<bool> long_vector = {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1};
  while (long_vector.size() != (1 << 5) - 1) {
    long_vector.push_back(0);
  }
  std::vector<bool> long_vector_answer = {1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1};
  while (long_vector_answer.size() != (1 << 5) - 1) {
    long_vector_answer.push_back(0);
  }
  std::vector<std::pair<std::vector<bool>, uint8_t>> bits = {
    {{0, 0, 1}, 2},
    {{0, 0, 0, 0, 0, 0, 0}, 3},
    {{0, 0, 1, 0, 1, 0, 0}, 3},
    {{0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0}, 4},
    {long_vector, 5}
  };

  std::vector<std::vector<bool>> results = {
    {1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0},
    {1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0},
    long_vector_answer
  };

  for (int i = 0; i < bits.size(); i++) {
    CalculateParityBits(bits[i].first, bits[i].second);
    ASSERT_EQ(bits[i].first, results[i]);
  } 
}


TEST(HammingCodesTestSuite, CheckErrorTest) {
  std::vector<std::pair<std::vector<bool>, uint8_t>> bits = {
    {{1, 1, 1}, 2},
    {{0, 1, 0, 0, 0, 0, 0}, 3},
    {{0, 0, 1, 0, 1, 0, 0}, 3},
    {{1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0}, 4},
  };

  std::vector<std::vector<bool>> prev_parity = {
    {1, 1},
    {0, 0, 0},
    {0, 1, 1},
    {1, 0, 0, 1},
  };

  std::vector<bool> results = {1, 0, 0, 1};

  for (int i = 0; i < bits.size(); i++) {
    ASSERT_EQ(results[i], CheckParityBits(bits[i].first, prev_parity[i], bits[i].second));
  }
}

TEST(HammingCodesTestSuite, FixErrorTest) {
  std::vector<std::pair<std::vector<bool>, uint8_t>> bits = {
    {{0, 0, 0}, 2},
    {{0, 0, 0}, 2},
    {{0, 0, 1}, 2},
    {{0, 0, 0, 0, 1, 0, 0}, 3},
    {{0, 0, 1, 0, 1, 0, 0}, 3},
    {{0, 0, 1, 0, 1, 1, 1}, 3},
    {{0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0}, 4},
    {{0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0}, 4},
  };
  std::vector<std::vector<bool>> prev_parity = {
    {1, 0},
    {1, 1},
    {1, 1},
    {0, 0, 0},
    {0, 1, 1},
    {1, 0, 0},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
  };

  std::vector<int32_t> results = {kNoError, kErrorFixed, kNoError,
                                  kErrorFixed, kNoError, kErrorFixed, 
                                  kNoError, kErrorFixed};

  for (int i = 0; i < bits.size(); i++) {
    CalculateParityBits(bits[i].first, bits[i].second);
    ASSERT_EQ(results[i], FixError(bits[i].first, prev_parity[i], bits[i].second));
  }
}

TEST(HammingCodesTestSuite, DefaultEnDecondingTest) {
  std::vector<uint8_t> bytes = {98, 120, 156, 0, 255};
  for (int i = 0; i < bytes.size(); i++) {
    uint8_t current_byte = bytes[i];
    std::vector<bool> bits_first_part(4);
    for (int i = 0; i < 4; i++) {
      bits_first_part[i] = current_byte & (1 << i);
    }
    uint8_t encoded_first_part = DefaultEncoding(bits_first_part);

    std::vector<bool> bits_second_part(4);
    for (int i = 4; i < 8; i++) {
      bits_second_part[i - 4] = current_byte & (1 << i);
    }
    uint8_t encoded_second_part = DefaultEncoding(bits_second_part);

    uint8_t decoded = DefaultDecoding(encoded_first_part) + DefaultDecoding(encoded_second_part) * (1 << 4);
    ASSERT_TRUE(current_byte == decoded);
  }
}

TEST(HammingCodesTestSuite, WriteGetEncodedSymbolTest) {
  std::cout << "here\n";
  std::vector<uint8_t> bytes = {98, 120, 156, 0, 255};
  std::string filename = "test_write_get.txt";

  for (int i = 0; i < bytes.size(); i++) {
    std::cout << "i " << i << "\n";
    WriteTwoParts(filename, bytes[i]);
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    ASSERT_EQ(GetTwoBytes(file), bytes[i]);
    file.close();
    std::remove(filename.c_str());
  }
}


// archiver tests
TEST(ArchiverTestSuite, CheckArchiverTest) {
  std::vector<std::string> files = {
    "../test_files/1.txt",
    "../test_files/result.bmp",
    "../test_files/merged_arc.haf",
    "../test_files/test_arc.haf",
    "../test_files/b.cpp"
  };

  std::vector<bool> results = {0, 0, 1, 1, 0};

  for (int i = 0; i < files.size(); i++) {
    std::ifstream file(files[i], std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "NO\n";
        continue;
    }
    ASSERT_EQ(results[i], CheckType(file));
    file.close();
  }
}

TEST(ArchiverTestSuite, PackUnpackFilesTest) {
  std::vector<std::string> files = {
    "../test_files/1.txt",
    "../test_files/2.txt",
    "../test_files/3.txt",
  };
  std::vector<std::string> renamed_files = {
    "../test_files/1_prev.txt",
    "../test_files/2_prev.txt",
    "../test_files/3_prev.txt",
  };
  std::vector<uint8_t> parity_bits = {2, 3, 4, 5, 6}; 
  int32_t cnt = 0;
  std::vector<std::pair<std::string, uint8_t>> errors;
  for (const auto& parity: parity_bits) {
    Archiver arc;
    std::string archive_name = "../test_files/test_pack_unpack_"; 
    archive_name += (parity + '0');
    archive_name += "_bits.haf";

    arc.Create(archive_name, parity, files, false);

    for (int i = 0; i < files.size(); i++) {
      std::rename(files[i].c_str(), renamed_files[i].c_str());
    }

    arc.ExtractAll(archive_name);
    for (int i = 0; i < files.size(); i++) {
      ASSERT_TRUE(CompareFiles(files[i], renamed_files[i]));
    }

    // delete extracted files
    for (int i = 0; i < files.size(); i++) {
      std::remove(files[i].c_str());
    }
    // rename input data
    for (int i = 0; i < files.size(); i++) {
      std::rename(renamed_files[i].c_str(), files[i].c_str());
    }
  }
}


TEST(ArchiverTestSuite, DifferentFileTypesTest) {
  std::vector<std::string> files = {
    "../test_files/1.txt",
    "../test_files/result.bmp",
    "../test_files/b.cpp",
  };
  std::vector<std::string> renamed_files = {
    "../test_files/1_prev.txt",
    "../test_files/result_prev.bmp",
    "../test_files/b_prev.cpp",
  };
  uint8_t parity = 3;
  Archiver arc;
  std::string archive_name = "../test_files/test_different_file_types_"; 
  archive_name += (parity + '0');
  archive_name += "_bits.haf";

  arc.Create(archive_name, parity, files, false);

  for (int i = 0; i < files.size(); i++) {
    std::rename(files[i].c_str(), renamed_files[i].c_str());
  }

  arc.ExtractAll(archive_name);
  for (int i = 0; i < files.size(); i++) {
    ASSERT_TRUE(CompareFiles(files[i], renamed_files[i]));
  }

  // delete extracted files
  for (int i = 0; i < files.size(); i++) {
    std::remove(files[i].c_str());
  }
  // rename input data
  for (int i = 0; i < files.size(); i++) {
    std::rename(renamed_files[i].c_str(), files[i].c_str());
  }
}


TEST(ArchiverTestSuite, FilesListTest) {
  std::vector<std::string> files = {
    "../test_files/1.txt",
    "../test_files/2.txt",
    "../test_files/b.cpp",
  };
  uint8_t parity = 3;
  Archiver arc;
  std::string archive_name = "../test_files/test_files_list_"; 
  archive_name += (parity + '0');
  archive_name += "_bits.haf";

  arc.Create(archive_name, parity, files, false);

  std::vector<std::tuple<std::string, double, uint8_t>> files_list;
  bool search_result = arc.SearchInfo(files_list, archive_name);

  for (int i = 2; i < files_list.size(); i++) {
    ASSERT_EQ(std::get<0>(files_list[i]), files[i - 2]);
  }
  std::remove(archive_name.c_str());
}

TEST(ArchiverTestSuite, AddFilesTest) {
  std::vector<std::string> files = {
    "../test_files/1.txt",
    "../test_files/2.txt",
  };

  std::vector<std::string> add_files = {
    "../test_files/b.cpp",
    "../test_files/3.txt",
  };

  std::vector<std::string> result = {
    "../test_files/1.txt",
    "../test_files/2.txt",
    "../test_files/b.cpp",
    "../test_files/3.txt",
  };

  uint8_t parity = 3;
  Archiver arc;
  std::string archive_name = "../test_files/test_add_files_"; 
  archive_name += (parity + '0');
  archive_name += "_bits.haf";

  arc.Create(archive_name, parity, files, false);
  
  uint8_t arc_parity = arc.GetNumberOfParityBits(archive_name);

  for (const auto& name: add_files) {
    arc.AddFile(archive_name, arc_parity, name);
  }

  std::vector<std::tuple<std::string, double, uint8_t>> files_list;
  bool search_result = arc.SearchInfo(files_list, archive_name);

  for (int i = 2; i < files_list.size(); i++) {
    ASSERT_EQ(std::get<0>(files_list[i]), result[i - 2]);
  }

  std::remove(archive_name.c_str());
}

TEST(ArchiverTestSuite, DeleteFilesTest) {
  std::vector<std::string> files = {
    "../test_files/1.txt",
    "../test_files/2.txt",
    "../test_files/b.cpp",
    "../test_files/3.txt",
  };

  std::vector<std::string> delete_files = {
    "../test_files/3.txt",
    "../test_files/2.txt",
  };

  std::vector<std::string> result = {
    "../test_files/1.txt",
    "../test_files/b.cpp",
  };

  uint8_t parity = 3;
  Archiver arc;
  std::string archive_name = "../test_files/test_delete_files_"; 
  archive_name += (parity + '0');
  archive_name += "_bits.haf";

  arc.Create(archive_name, parity, files, false);

  arc.DeleteFiles(archive_name, delete_files);

  std::vector<std::tuple<std::string, double, uint8_t>> files_list;
  bool search_result = arc.SearchInfo(files_list, archive_name);

  for (int i = 2; i < files_list.size(); i++) {
    ASSERT_EQ(std::get<0>(files_list[i]), result[i - 2]);
  }

  std::remove(archive_name.c_str());
}


TEST(ArchiverTestSuite, ExtractFilesTest) {
  std::vector<std::string> files = {
    "../test_files/1.txt",
    "../test_files/2.txt",
    "../test_files/b.cpp",
    "../test_files/3.txt",
  };

  std::vector<std::string> extract_files = {
    "../test_files/3.txt",
    "../test_files/2.txt",
  };

  std::vector<std::string> renamed_extract_files = {
    "../test_files/2_prev.txt",
    "../test_files/3_prev.txt",
  };

  std::vector<std::string> result = {
    "../test_files/1.txt",
    "../test_files/b.cpp",
  };

  uint8_t parity = 3;
  Archiver arc;
  std::string archive_name = "../test_files/test_extract_files_"; 
  archive_name += (parity + '0');
  archive_name += "_bits.haf";

  arc.Create(archive_name, parity, files, false);

  for (int i = 0; i < extract_files.size(); i++) {
    std::rename(extract_files[i].c_str(), renamed_extract_files[i].c_str());
  }

  arc.ExtractFiles(archive_name, extract_files, false);

  std::vector<std::tuple<std::string, double, uint8_t>> files_list;
  bool search_result = arc.SearchInfo(files_list, archive_name);

  for (int i = 2; i < files_list.size(); i++) {
    ASSERT_EQ(std::get<0>(files_list[i]), result[i - 2]);
  }

  for (int i = 0; i < extract_files.size(); i++) {
    ASSERT_TRUE(CompareFiles(extract_files[i], renamed_extract_files[i]));
  }

  for (int i = 0; i < extract_files.size(); i++) {
    std::remove(extract_files[i].c_str());
  }

  for (int i = 0; i < extract_files.size(); i++) {
    std::rename(renamed_extract_files[i].c_str(), extract_files[i].c_str());
  }

  std::remove(archive_name.c_str());
}

TEST(ArchiverTestSuite, MergeArchiversTest) {
  uint8_t parity = 3;

  Archiver arc;
  std::string first_name = "../test_files/test_first.haf";
  std::string second_name = "../test_files/test_second.haf";
  std::string archive_name = "../test_files/test_merge_archivers.haf";

  std::vector<std::string> files1 = {
    "../test_files/1.txt",
    "../test_files/2.txt",
  };

  std::vector<std::string> files2 = {
    "../test_files/b.cpp",
    "../test_files/3.txt",
  };

  std::vector<std::string> result = {
    "../test_files/1.txt",
    "../test_files/2.txt",
    "../test_files/b.cpp",
    "../test_files/3.txt",
  };

  arc.Create(first_name, parity, files1, false);
  arc.Create(second_name, parity, files2, false);

  arc.Merge(archive_name, first_name, second_name);

  std::vector<std::tuple<std::string, double, uint8_t>> files_list;
  bool search_result = arc.SearchInfo(files_list, archive_name);

  for (int i = 2; i < files_list.size(); i++) {
    ASSERT_EQ(std::get<0>(files_list[i]), result[i - 2]);
  }
  
  std::remove(first_name.c_str());
  std::remove(second_name.c_str());
  std::remove(archive_name.c_str());
}
