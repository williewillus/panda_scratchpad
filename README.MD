# PMTester
PMTester is a record and replay tool for testing crash-consistency bugs in Persistent memory file systems.
PMTester makes use of the PANDA instrumentation tool for QEMU to track the writes to memory.

	
## Usage
```
1. Setup panda
	1. I made a few hacks in the panda repo to append a terminating string to detect end of communication 
	   with the QEMU monitor (Original PANDA repo : [PANDA](https://www.github.com/panda-re/panda))
	   git clone https://github.com/jayashreemohan29/panda.git
	2. git checkout hack
	3. cd panda
	4. ./configure
	5. cd ..
	6. git clone https://github.com/utsaslab/crashmonkey-PM.git
	7. cd panda
	8. ./build.sh  --extra-plugins-path=../../crashmonkey-PM/personal_plugins --target-list=x86_64-softmmu
	
	Now PANDA is setup and ready.

2. Setup VM
	1. Spin up a VM running Ubuntu 16.04 or copy the VM image in my home directory on Chennai machine : 
	/home/jayashree/projects/panda_scratchpad/vm_new.img
	(This VM is a minimal installation of Arch Linux and has a kernel that supports both NOVA and PMFS, kernel 4.13. It also has 2 pmem devices - the first one from 1024MB-1152MB, and the second from 1152MB-1280MB (128MB each))

3. Launch the VM
	1. cd crashmonkey-PM
	2. ./launch_headless.sh vm_new.img 2G
		This command launches the VM with 2GB memory, and forwards requests on port 2222, and
		opens up the qemu monitor on port 4444
		
4. Login to the VM and run PMTester
	1. Start the VM:
		ssh -p 2222 root@localhost
	2. Clone the repo : git clone https://github.com/utsaslab/crashmonkey-PM.git
	3. cd crashmonkey-PM
	4. make
	5. Setup mount points and ramdisk
		i. mkdir /mnt/ramdisk
		ii. mount -t tmpfs -o size=256m tmpfs /mnt/ramdisk
		iii. mkdir /mnt/pmem0; mkdir /mnt/pmem1
		iv. We assume by default that /dev/pmem0 and /dev/pmem1 devices exist. If they don't,
		pass the device paths to the harness using -d <record_dev> and -r <replay_dev> flags.
	6. There's one sample workload j-lang150 which does an unaligned write followed by fsync. 
	   To run it on NOVA(default),
	   ./harness build/tests/j-lang150.so
	   
	7. To run it on pmfs/ext4-dax, use the -f flag
	   ./harness -f pmfs build/tests/j-lang150.so
	   ./harness -f dax build/tests/j-lang150.so
	   ./harness -f NOVA build/tests/j-lang150.so
	  (Sorry, the FS names are case sensitive for now. So ensure you type NOVA in caps)
	
	(Default values for tracking range, FS size etc can be found in the file src/main.cpp.
	You can pass these in as flags to the harness.
	The default IP for commmunicatoing to the QEMU monitor is set to that of chennai machine
	So be sure to override it by passing the IP of your machine using -i flag)
	
	8. After the run, a 'diff' file will be generated in the root directory which mentions the
	errors that resulted in the bug if the test fails. 
```



# Other useful references

## QEMU and PANDA Setup
* Clone this repo: https://github.com/utsaslab/crashmonkey-PM.git
* Clone panda: github.com/panda-re/panda
  * Build panda according to the instructions for Ubuntu 16.04
    * When executing the build.sh file, append these options: `./build.sh --extra_plugins_path=<path_to_this_repo>/personal_plugins --target-list=x86_64-softmmu`. This includes our plugin for compilation and only compiles QEMU for x86_64
  * If the compilation fails saying something about plugins/network, disable that plugin by removing the line "network" from the `<panda_repo>/panda/plugins/config.panda` file
  * If the compilation throws errors with dtc modules, run: `git submodule update --init dtc` and then do `./configure`

## VM Setup and Usage
* Retrieve the vm image: `scp <user>@chennai.csres.utexas.edu:/home/vincent/SAS_RESEARCH/vm_clean.img <dest>/vm.img`
  * It is a very minimal installation of Arch Linux with a passwordless root account, running a build of NOVA
  * The VM image is big, so you can also download Arch Linux and install it yourself if you don't want to download that much
* Boot QEMU using the launch.sh or launch_headless.sh scripts in this repo
  * It assumes the panda repo is a sibling directory of this repo
  * The VM can be accessed after it boots in about 30s via ssh: `ssh -p 2222 root@localhost`
  * Enter the QEMU monitor by doing `telnet localhost 4444`
  * From here, you can load the writetracker plugin using `load_plugin writetracker start=<start_address>,end=<end_address>`
  * The plugin can be stopped and unloaded by using `list_plugins` to get writetracker's index (first column), then doing `unload_plugin <idx>`
  * Exit the monitor using ctrl-esc, then `telnet> close`
* Whenever the plugin is unloaded, it writes all of its tracked writes into `wt.out`, overwriting of it exists already.
* The out file can be read with the `src/reader.cpp` program (very rough frontend right now, and likely to change)

## Compiling NOVA
* Clone nova outside the VM: `git clone https://www.github.com/nvsl/linux-nova`
  * cd into it and `make clean mrproper defconfig nconfig`
  * in the gui that pops up:
    * Set General Setup > Local Version to "-nova"
    * Under processor type and features, set the following to "\*":
      * Transparent Hugepage Support (CONFIG_TRANSPARENT_HUGEPAGE)
      * Support non-standard NVDIMMs... (CONFIG_X86_PMEM_LEGACY)
      * Allow for memory hot-add (CONFIG_MEMORY_HOTPLUG)
      * Allow for memory hot-remove (CONFIG_MEMORY_HOTREMOVE)
      * Device memory hotplug support (CONFIG_ZONE_DEVICE)
    * Set Device Drivers > NVDIMM and DAX and all their subitems to "\*"    
      * NVDIMM should have at least 5 items, DAX 2. otherwise a config was missed in the processor type section above
    * Set File Systems > Direct Access (CONFIG_FS_DAX) to "\*" and NOVA (CONFIG_FS_NOVA) to "\*"
  * compile the kernel with `make -j5`  
* Transfer kernel into the (running) VM using `scp -p 2222 arch/x86_64/boot/bzImage root@localhost:/boot/vmlinuz-linux-nova`
* Reboot the VM from SSH
