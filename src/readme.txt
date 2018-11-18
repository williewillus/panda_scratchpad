Scripts and their usages:
* take_snapshot.sh: Takes clean snapshot of first pmem device into dump.snap
* apply_snapshot.sh: Writes dump.snap into the second pmem device
* compare_devices.sh: Verifies that the first and second pmem devices have identical contents
* clear_pmem.sh: Zeroes out both pmem devices

Sample Workflow (all in vm, <> outside of vm):
* clear_pmem.sh to zero out pmem
* mount -t NOVA -o init /dev/pmem0 /mnt/nova
* umount /dev/pmem0
* ./take_snapshot.sh
* mount -t NOVA /dev/pmem0 /mnt/nova
* <enable writetracker in QEMU monitor: load_plugin writetracker>
* echo "hello world" > /mnt/nova/test
* umount /dev/pmem0
* <disable writetracker, producing wt.out: unload_plugin 0 (use list_plugins to find index, most likely will be 0)>
* ./apply_snapshot.sh
* <enable replayer with wt.out, then unload it after it finishes>
* ./compare_devices.sh
