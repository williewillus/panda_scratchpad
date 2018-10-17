#include "vm.h"
#include <cstdio>

int main(int argc, char** argv) {
  auto r = vm::run_command("ls .");
  printf("%s\n", r.c_str());
  vm::dump_pmem();
}
