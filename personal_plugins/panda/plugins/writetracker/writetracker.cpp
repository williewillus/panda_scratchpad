#include <iostream>
#include <fstream>
#include <cstdlib>
#include <memory>

#include "panda/plugin.h"

static target_ulong range_start;
static target_ulong range_end;

static unsigned long writes = 0;

static std::unique_ptr<std::ofstream> output;

extern "C" int mem_write_callback(CPUState *env, target_ulong pc, target_ulong addr,
                       target_ulong size, void *buf) {
    if (addr >= range_start && addr < range_end) {
        writes++;
	*output << "got a write at " << std::hex << addr << std::endl;
    }
    return 0;
}

static bool is_flush(CPUState *env, target_ulong pc) {
  uint8_t insn[3];
  int err = panda_virtual_memory_read(env, pc, insn, 3);
  if (err < 0) {
    std::cout << "Error reading insns!" << std::endl;
    return false;
  }

  if (insn[0] == 0x0F && insn[1] == 0xAE) // clflush
    return true;
  
  if (insn[0] == 0x66 && insn[1] == 0x0F && insn[2] == 0xAE) // clflushopt and clwb (distinguished by modrm byte which we do not examine)
    return true;

  return false;
}

extern "C" bool translate_callback(CPUState *env, target_ulong pc) {
  return is_flush(env, pc);
}

extern "C" int exec_callback(CPUState *env, target_ulong pc) {
  if (is_flush(env, pc)) {
    *output << "flush at ???" << std::endl;// TODO extract the address by examining the instruction and looking in the register it points to
  } else {
    // std::cout << "WARNING: exec callback running for non-flush!" << std::endl;
  }
  return 0;
}

extern "C" bool init_plugin(void *self) {
    panda_arg_list *args = panda_get_args("writetracker");
    range_start = panda_parse_ulong_opt(args, "start", 0x40000000, "Start address tracking range, default 1G");
    range_end = panda_parse_ulong_opt(args, "end", 0x80000000, "End address (exclusive) of tracking range, default 2G"); 
    std::cout << "writetracker loading" << std::endl;
    std::cout << "tracking range [" << std::hex << range_start << ", " << std::hex << range_end << ")" << std::endl;

    // Need this to get EIP with our callbacks
    panda_enable_precise_pc();
    // Enable memory logging
    panda_enable_memcb();

    panda_cb pcb {};
    pcb.insn_translate = translate_callback;
    panda_register_callback(self, PANDA_CB_INSN_TRANSLATE, pcb);

    pcb = {};
    pcb.insn_exec = exec_callback;
    panda_register_callback(self, PANDA_CB_INSN_EXEC, pcb);

    pcb = {};
    pcb.phys_mem_before_write = mem_write_callback;
    panda_register_callback(self, PANDA_CB_PHYS_MEM_BEFORE_WRITE, pcb);

    output = std::unique_ptr<std::ofstream>(new std::ofstream("wt.out", std::ios::binary));
    return true;
}

extern "C" void uninit_plugin(void *self) {
    std::cout << "writetracker unloading" << std::endl;
    std::cout << "writes to range " << std::dec << writes << std::endl;
    output.reset();
}
