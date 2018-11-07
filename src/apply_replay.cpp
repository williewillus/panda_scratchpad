#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

  int pmem_fd = open("/dev/pmem1", O_WRONLY);
  if (pmem_fd == -1) {
    perror("Couldn't open pmem1");
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
      if (lseek(pmem_fd, offset, SEEK_SET) == -1) {
	perror("seek failed");
	return 1;
      }
      write_data.resize(write_size);
      input.read(write_data.data(), write_size);
      if (write(pmem_fd, write_data.data(), write_size) != write_size) {
	std::cerr << "Didn't write enough to pmem" << std::endl;
	return 1;
      }
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
