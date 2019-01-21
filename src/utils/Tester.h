#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>

#include "FSCommands.h"
#include "../wrapper/DiskMod.h"
#include "../tests/BaseTestCase.h"
#include "../results/DataTestResult.h"
#include "../results/TestSuiteResult.h"
#include "ClassLoader.h"
#include "../checker/DiskContents.h"

#define SUCCESS		0
#define SNAP_ERR	-1
#define RESTORE_ERR	-2
#define MNT_ERR		-3
#define FMT_ERR		-4
#define UMNT_ERR	-5


class Tester {

public:
  	
enum time_stats {
	PERMUTE_TIME,
	SNAPSHOT_TIME,
	BIO_WRITE_TIME,
	FSCK_TIME,
	TEST_CASE_TIME,
	MOUNT_TIME,
	TOTAL_TIME,
	NUM_TIME,
};


	Tester(const unsigned int device_size, const bool verbosity);
	~Tester();
  	const bool verbose = false;
  	void set_fs(const std::string fs_type);
  	void set_record_device(const std::string device_path);
  	void set_replay_device(const std::string device_path);
	
	int snapshot_device();
	int restore_snapshot();

	int mount_device(std::string device_path, std::string mount_point);
	int format_and_mount_device(std::string device_path, std::string mount_point);
	int umount_device();

	std::chrono::milliseconds get_timing_stat(time_stats timing_stat);
	void PrintTimingStats(std::ostream& os);
	void PrintTestStats(std::ostream& os);
	void StartTestSuite();
	void EndTestSuite();	

	int GetChangeData(const int fd);
	int CreateCheckpoint();

	int test_setup();
	int test_init_values(std::string mount_dir, long filesys_size);
	int test_run(const int change_fd, const int checkpoint);
	std::vector<std::chrono::milliseconds> test_check(const std::string device_path, std::ofstream &log);
	
	int test_load_class(const char* path);
	void test_unload_class();

	void cleanup_harness();

private:
	FSCommands * fs_commands_ = NULL;
	utils::ClassLoader<tests::BaseTestCase> test_loader;

	std::string fs;
	std::string record_device_path;
	std::string replay_device_path;
	const unsigned long int device_size;

	TestSuiteResult *current_test_suite_ = NULL;
	std::vector<TestSuiteResult> test_results_;
	std::chrono::milliseconds timing_stats[NUM_TIME] = {std::chrono::milliseconds(0)};

	bool record_dev_mounted = false;
	std::vector<std::vector<wrapper::DiskMod>> mods_;

	bool check_disk_and_snapshot_contents();

	};

std::ostream& operator<<(std::ostream& os, Tester::time_stats time);
	
#endif //TESTER



