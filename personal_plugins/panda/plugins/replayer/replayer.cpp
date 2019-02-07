#include <iostream>
#include <fstream>
#include <vector>

#include "panda/plugin.h"

static constexpr int WRITE = 0;
static constexpr int FLUSH = 1;
static constexpr int FENCE = 2;
static constexpr int CHECKPOINT = 3;

extern "C" bool init_plugin(void *self) {
    panda_arg_list *args = panda_get_args("replayer");
    auto base = panda_parse_ulong_opt(args, "base", 0x40000000, "Base physical address to replay at");
    auto file = panda_parse_string(args, "file", "wt.out");
    auto stop_checkpoint = panda_parse_ulong_opt(args, "checkpoint", 1, "Which (1-indexed) checkpoint to stop at");
    
    std::cout << "replayer starting at " << std::hex << base << std::endl;

  std::ifstream input(file, std::ios::binary);
  if (!input.good()) {
    std::cerr << "Cannot open file" << std::endl;
    return false;
  }

  uint64_t pc;
  int type;
  uint64_t offset;
  uint64_t write_size;
  std::vector<uint8_t> write_data;
  uint32_t last_checkpoint_seen = 0;

  while (input) {
    input.read(reinterpret_cast<char*>(&pc), sizeof(pc));
    input.read(reinterpret_cast<char*>(&type), sizeof(type));
    switch (type) {
    case WRITE: {
      input.read(reinterpret_cast<char*>(&offset), sizeof(offset));
      input.read(reinterpret_cast<char*>(&write_size), sizeof(write_size));
      write_data.resize(write_size);
      input.read((char*) write_data.data(), write_size);
      /*if (offset == 0x11c8) {
	std::cout << "Terminating early" << std::endl;
	return true;
      }*/
      panda_physical_memory_rw(base + offset, write_data.data(), write_size, true);
      std::cout << "[pc 0x" << std::hex << pc << "] write to offset " << offset << " addr " << base + offset << ", size " << write_size << std::endl;
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
    case CHECKPOINT: {
      last_checkpoint_seen++;
      if (last_checkpoint_seen == stop_checkpoint)
        goto done;
      else break;
    }
    }
  }

done:
  std::cout << "replay done" << std::endl;
  return true;
}

extern "C" void uninit_plugin(void *self) {}
