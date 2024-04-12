#include "archiver.h"
#include "ArgParser.cpp"

int main(int argc, char** argv) {
  ArgumentParser::ArgParser parser("my_parser");
  std::vector<std::string> files;
  std::string archiver_name;
  int32_t parity;

  parser.AddFlag('c', "create", "create new archiver");
  parser.AddFlag('C', "compress", "compress data");
  parser.AddIntArgument('p', "parity", "number of parity bits").StoreValue(parity).Default(3);
  parser.AddStringArgument("files", "input filename/archivers names").Positional().MultiValue().StoreValues(files);
  parser.AddStringArgument('f', "file", "archiver name").StoreValue(archiver_name);
  parser.AddFlag('l', "list", "list of files in archiver");
  parser.AddFlag('x', "extract", "extract files from archiver");
  parser.AddFlag('a', "append", "add file to archiver");
  parser.AddFlag('d', "delete", "delete file from archiver");
  parser.AddFlag('A', "concatenate", "merge two archivers");

  bool parse_result = parser.Parse(argc, argv);
  if (!parse_result) {
    std::cout << parser.HelpDescription() << std::endl;
    return 0;
  }

  Archiver arc;
  if (parser.GetFlag("create")) {
    if (parity < 2 || parity > 20) {
      std::cout << "Number of parity bits must be [2;20]";
    }
    else {
      // std::cout << "create\n";
      // std::cout << "compres " << parser.GetFlag("compress") << "\n";
      bool compress = parser.GetFlag("compress");
      arc.Create(archiver_name, parity, files, compress);
    }
  }
  else if (parser.GetFlag("extract")) {
    if (!files.size()) {
      arc.ExtractAll(archiver_name);
    }
    else {
      arc.ExtractFiles(archiver_name, files, false);
    }
  }
  else if (parser.GetFlag("delete")) {
    arc.DeleteFiles(archiver_name, files);
  }
  else if (parser.GetFlag("append")) {
    uint8_t current_parity = arc.GetNumberOfParityBits(archiver_name);
    for (const auto& name: files) {
      arc.AddFile(archiver_name, current_parity, name);
    }
  }
  else if (parser.GetFlag("concatenate")) {
    if (files.size() != 2) {
      std::cout << "Need two archivers\n";
    }
    else {
      arc.Merge(archiver_name, files[0], files[1]);
    }
  }

  if (parser.GetFlag("list")) {
    arc.Info(archiver_name);
  }
  
}