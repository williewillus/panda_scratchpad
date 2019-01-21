#ifndef HARNESS_SINGLE_TEST_INFO_H
#define HARNESS_SINGLE_TEST_INFO_H

#include "DataTestResult.h"
#include "FileSystemTestResult.h"


// Container for all things related to the crash state being tested including
// 1. bio prefix used to generate the state
// 2. permutation of bios in final epoch of state
// 3. results of user data consistency tests
// 4. results of file system consistency tests and repair(s)
class SingleTestInfo {
 public:
  enum ResultType {
    kPassed,
    kFsckFixed,
    kFsckRequired,
    kFailed
  };

  SingleTestInfo();
  void PrintResults(std::ostream& os) const;
  SingleTestInfo::ResultType GetTestResult() const;

  unsigned int test_num;
  tests::DataTestResult data_test;
  FileSystemTestResult fs_test;
};

std::ostream& operator<<(std::ostream& os, SingleTestInfo::ResultType rt);

#endif  // HARNESS_SINGLE_TEST_INFO_H
