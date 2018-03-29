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
	*output << std::hex << "[pc 0x" << pc << "] got a write at " << addr << std::endl;
    }
    return 0;
}

static int reg_from_rm(uint8_t rm) {
  switch(rm) {
    case 0: return R_EAX;
    case 1: return R_ECX;
    case 2: return R_EDX;
    case 3: return R_EBX;
    case 6: return R_ESI;
    case 7: return R_EDI;
    default: return -1; 
    // NOTE: Doesn't account for exotic prefixes (like using %ds or extended registers like %r8)
    //       Nor does this account for displacement or SIB suffixes

  }
}

static bool is_flush(CPUState *env, target_ulong pc, target_ulong* flush_addr_out) {
  auto x86_env = (CPUX86State *)((CPUState*) env->env_ptr);
  uint8_t insn[4];
  int err = panda_virtual_memory_read(env, pc, insn, 4);
  if (err < 0) {
    std::cout << "Error reading insns!" << std::endl;
    return false;
  }

  if (insn[0] == 0x0F && insn[1] == 0xAE)
  {
    uint8_t reg = (insn[2] >> 3) & 7;
    uint8_t rm = insn[2] & 7;
    auto target_reg = reg_from_rm(rm);
    if (reg == 7 && target_reg > -1) // clflush: 0f ae modrm.reg == 7
    {
       if (flush_addr_out) {
         target_ulong va = x86_env->regs[target_reg];
         *flush_addr_out = panda_virt_to_phys(env, va);
       }
       return true;
    }
  }
  
  if (insn[0] == 0x66 && insn[1] == 0x0F && insn[2] == 0xAE)
  {
    uint8_t reg = (insn[3] >> 3) & 7;
    uint8_t rm = insn[3] & 7;
    auto target_reg = reg_from_rm(rm);
    if ((reg == 6 || reg == 7) && target_reg > -1) // clwb and clflushopt: 66 0f ae modrm.reg == 6 or 7
    {
       if (flush_addr_out) {
         target_ulong va = x86_env->regs[target_reg]; 
         *flush_addr_out = panda_virt_to_phys(env, va);
       }
       return true;
    }
  }

  return false;
}

extern "C" bool translate_callback(CPUState *env, target_ulong pc) {
  return is_flush(env, pc, nullptr);
}

extern "C" int exec_callback(CPUState *env, target_ulong pc) {
  target_ulong flush_addr = 0xdeadbeef;
  if (is_flush(env, pc, &flush_addr)) {
    *output << std::hex << "[pc 0x" << pc << "] flush at " << flush_addr << std::endl;
  } else {
    std::cout << "WARNING: exec callback running for non-flush!" << std::endl;
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
    *output << "writetracker unloading" << std::endl;
    *output << "writes to range " << std::dec << writes << std::endl;
    output.reset();
}
