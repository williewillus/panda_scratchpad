Scripts and their usages:
* take_snapshot.sh: Takes clean snapshot of first pmem device into dump.snap
* apply_snapshot.sh: Writes dump.snap into the second pmem device
* apply_replay.cpp: Takes wt.out and replays its operations onto the second pmem device
* compare_devices.sh: Verifies that the first and second pmem devices have identical contents
* clear_pmem.sh: Zeroes out both pmem devices

Sample Workflow (all in vm unless noted):
* clear_pmem.sh to zero out pmem
* mount -t NOVA -o init /dev/pmem0 /mnt/nova
* umount /dev/pmem0
* ./take_snapshot.sh
* mount -t NOVA /dev/pmem0 /mnt/nova
* <enable writetracker in QEMU monitor: load_plugin writetracker>
* echo "hello world" > /mnt/nova/test
* umount /dev/pmem0
* <disable writetracker, producing wt.out: unload_plugin 0 (use list_plugins to find index, most likely will be 0)>
* (TODO improve) get wt.out into the vm using scp or other
* ./apply_snapshot.sh
* apply_replay wt.out
* ./compare_devices.sh

