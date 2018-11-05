"Research is what I'm doing when I don't know what I'm doing"

- Experimentation with the [PANDA](https://www.github.com/panda-re/panda) instrumentation tools for QEMU

# QEMU Setup
* Clone this repo: github.com/williewillus/panda_scratchpad
* Clone panda: github.com/panda-re/panda
  * Build panda according to the instructions for Ubuntu 16.04
    * When executing the build.sh file, append these options: `./build.sh --extra_plugins_path=<path_to_this_repo>/personal_plugins --target-list=x86_64-softmmu`. This includes our plugin for compilation and only compiles QEMU for x86_64.
  * If the compilation fails saying something about plugins/network, disable that plugin by removing the line "network" from the `<panda_repo>/panda/plugins/config.panda` file.

# VM Setup and Usage
* Retrieve the vm image: `scp <user>@chennai.csres.utexas.edu:/home/vincent/SAS_RESEARCH/vm_clean.img <dest>/vm.img`
  * It is a very minimal installation of Arch Linux with a passwordless root account, running a build of NOVA on Linux 4.13
  * The VM image is big, so you can also download Arch Linux and install it yourself if you don't want to download that much
* Boot QEMU using the launch.sh or launch_headless.sh scripts in this repo
  * It assumes the panda repo is a sibling directory of this repo
  * The VM can be accessed after it boots in about 30s via ssh: `ssh -p 2222 root@localhost`
  * Enter the QEMU monitor by doing `telnet localhost 4444`
  * From here, you can load the writetracker plugin using `load_plugin writetracker start=<start_address>,end=<end_address>`
  * The plugin can be stopped and unloaded by using `list_plugins` to get writetracker's index (first column), then doing `unload_plugin <idx>`
  * Exit the monitor using ctrl-esc, then `telnet> close`

# Compiling NOVA
* NOTE: Currently, I'm having troubles getting NOVA master (4.18) to run in the VM properly (the pmem devices are not being recognized). For now, we should just continue running the vm with the existing 4.13 kernel

* Clone nova outside the VM: `git clone https://www.github.com/nvsl/linux-nova`
  * cd into it and `make clean mrproper defconfig nconfig`
  * in the gui that pops up:
    * Set General Setup > Local Version to "-nova"
    * Set Processor Type and Features > Transparent Hugepage Support (CONFIG_TRANSPARENT_HUGEPAGE) and Support non-standard NVDIMMs... (CONFIG_X86_PMEM_LEGACY) to "\*"
    * Set Device Drivers > NVDIMM and DAX and all their subitems to "\*"    
    * Set File Systems > Direct Access (CONFIG_FS_DAX) to "\*" and NOVA (CONFIG_FS_NOVA) to "\*"
  * compile the kernel with `make -j5`  
* Transfer kernel into the (running) VM using `scp -p 2222 arch/x86_64/boot/bzImage root@localhost:/boot/vmlinuz-linux-nova`
* Reboot the VM from SSH
