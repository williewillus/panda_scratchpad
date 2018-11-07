Scripts and their usages:
* take_snapshot.sh: Takes clean snapshot of first pmem device into dump.snap
* apply_clean.sh: Writes dump.snap into the second pmem device
* apply_replay: Takes wt.out and replays its operations onto the second pmem device
* compare_devices.sh: Verifies that the first and second pmem devices have identical contents
* clear_pmem.sh: Zeroes out both pmem devices

Workflow (inside the VM, all steps should be performed with NOVA unmounted):
* Initialize the desired initial state of the devices, using clear_pmem.sh if needed
* Use take_snapshot.sh to take a snapshot of the initial state of the first device
* Perform the workload with writetracker on, producing a file wt.out
* (Todo improve this) Get wt.out into the VM
* Apply the initial state snapshot to the second device using apply_snapshot.sh
* Replay wt.out on top of the second device using apply_replay
* (Temporary sancheck) use compare_devices.sh to make sure the devices have matching contents
