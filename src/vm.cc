#include <cstdio>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <array>
#include "vm.h"

namespace vm {
std::string run_command(const std::string& command) {
  std::ostringstream cmdbuilder;
  cmdbuilder << "ssh -p 2222 root@localhost " << command;
  std::string finalcmd = cmdbuilder.str();

  // Ref SO: https://stackoverflow.com/questions/478898/
  std::array<char, 128> buf;
  std::string result;
  std::shared_ptr<FILE> pipe(popen(finalcmd.c_str(), "r"), pclose);
  if (!pipe)
    throw std::runtime_error("Couldn't launch command");
  while (!feof(pipe.get())) {
    if (fgets(buf.data(), 128, pipe.get()))
      result += buf.data();
  }
  return result;
}

void dump_pmem() {
  run_command("sh /root/dump_pmem.sh");
  std::system("scp -P 2222 root@localhost:/root/dump /tmp/dump_scratch"); 
}

void restore_pmem() {
  std::system("scp -P 2222 /tmp/restore_scratch root@localhost:/root/dump");
  run_command("sh /root/restore_pmem.sh"); 
}
}
