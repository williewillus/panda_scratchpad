#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>

static const int WRITE = 0;
static const int FLUSH = 1;
static const int FENCE = 2;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Give input file (wt.out)" << std::endl;
    return 1;
  }

  std::ifstream input(argv[1], std::ios::binary);
  if (!input.good()) {
    std::cerr << "Cannot open file" << std::endl;
    return 1;
  }

  uint64_t pc;
  int type;
  uint64_t offset;
  uint64_t write_size;
  std::vector<char> write_data;

  while (input) {
    input.read(reinterpret_cast<char*>(&pc), sizeof(pc));
    input.read(reinterpret_cast<char*>(&type), sizeof(type));
    switch (type) {
    case WRITE: {
      input.read(reinterpret_cast<char*>(&offset), sizeof(offset));
      input.read(reinterpret_cast<char*>(&write_size), sizeof(write_size));
      write_data.resize(write_size);
      input.read(write_data.data(), write_size);
      std::cout << "[pc 0x" << std::hex << pc << "] write to offset " << offset << ", size " << write_size << std::endl;
      break;
    }
    case FLUSH: {
      input.read(reinterpret_cast<char*>(&offset), sizeof(offset));
      std::cout << "[pc 0x" << std::hex << pc << "] flush at VA " << offset << std::endl;
      break;
    }
    case FENCE: {
      std::cout << "[pc 0x" << std::hex << pc << "] mfence or sfence" << std::endl;
      break;
    }
    }
  }
  std::cout << "done?" << std::endl;
}
