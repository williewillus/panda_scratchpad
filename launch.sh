../panda/x86_64-softmmu/qemu-system-x86_64 -drive file=vm.img,format=qcow2 -m 3G -monitor stdio -net user,hostfwd=tcp::2222-:22 -net nic "$@" 
