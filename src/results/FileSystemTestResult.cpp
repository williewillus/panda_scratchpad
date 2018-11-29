#include "FileSystemTestResult.h"


using std::ostream;

FileSystemTestResult::FileSystemTestResult() {
  error_summary_ = FileSystemTestResult::kCheckNotRun;
}

void FileSystemTestResult::ResetError() {
  error_summary_ = FileSystemTestResult::kCheckNotRun;
}

void FileSystemTestResult::SetError(ErrorType err) {
  error_summary_ = error_summary_ | err;
}

unsigned int FileSystemTestResult::GetError() const {
  return error_summary_;
}

void FileSystemTestResult::PrintErrors(ostream& os) const {
  if (error_summary_ == 0) {
    os << FileSystemTestResult::kCheckNotRun;
    return;
  }

  unsigned int noted_errors = error_summary_;
  unsigned int shift = 0;
  while (noted_errors != 0) {
    if (noted_errors & 1) {
      os << (FileSystemTestResult::ErrorType) (1 << shift) << " ";
    }
    ++shift;
    noted_errors >>= 1;
  }
}

ostream& operator<<(ostream& os, FileSystemTestResult::ErrorType err) {
  switch (err) {
    case FileSystemTestResult::kCheckNotRun:
      os << "fsck_not_run";
      break;
    case FileSystemTestResult::kClean:
      os << "fsck_no_errors";
      break;
    case FileSystemTestResult::kUnmountable:
      os << "unmountable_file_system";
      break;
    case FileSystemTestResult::kCheck:
      os << "fsck_error";
      break;
    case FileSystemTestResult::kFixed:
      os << "file_system_fixed";
      break;
    case FileSystemTestResult::kSnapshotRestore:
      os << "restoring_snapshot";
      break;
    case FileSystemTestResult::kBioWrite:
      os << "writing_crash_state";
      break;
    case FileSystemTestResult::kOther:
      os << "other_error";
      break;
    case FileSystemTestResult::kKernelMount:
      os << "kernel_mount";
      break;
    case FileSystemTestResult::kCheckUnfixed:
      os << "unfixed_fsck_errors";
      break;
    default:
      os.setstate(std::ios_base::failbit);
  }
  return os;
}


