#ifndef HARNESS_TEST_SUITE_RESULT_H
#define HARNESS_TEST_SUITE_RESULT_H

#include <vector>

#include <iostream>

#include "SingleTestInfo.h"

struct ResultSet {
  unsigned int num_failed = 0;
  unsigned int num_passed_fixed = 0;
  unsigned int num_passed = 0;
  unsigned int total_tests = 0;

  unsigned int fsck_required = 0;
  unsigned int old_file_persisted = 0;
  unsigned int file_missing = 0;
  unsigned int file_data_corrupted = 0;
  unsigned int file_metadata_corrupted = 0;
  unsigned int incorrect_block_count = 0;
  unsigned int other = 0;

  unsigned int auto_check_failed = 0;
};

class TestSuiteResult {
 public:
  void TallyReorderingResult(SingleTestInfo &r);
  void TallyTimingResult(SingleTestInfo &r);
  unsigned int GetCompleted() const;
  unsigned int GetReorderingCompleted() const;
  unsigned int GetTimingCompleted() const;
  void PrintResults(std::ostream& os) const;

 private:
  ResultSet reordering_results_;
  ResultSet timing_results_;

  void TallyResult(SingleTestInfo &r, ResultSet &set);
};


#endif  // HARNESS_TEST_SUITE_RESULT_H
