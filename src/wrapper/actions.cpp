#include "actions.h"


namespace wrapper {

int Checkpoint() {
  // Issue an unusual, old instruction so that writetracker can pick it up and record a checkpoint in wt.out
  // fdisi is an old "disable floating point interrupts" instruction that is a nop on modern CPUs
  asm volatile("fdisi");
  return 0;
}

} // wrapper
