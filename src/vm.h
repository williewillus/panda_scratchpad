#pragma once
#include <string>

namespace vm {

std::string run_command(const std::string& command);
void dump_pmem();
void restore_pmem();

}
