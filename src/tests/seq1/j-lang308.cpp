#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <errno.h>
#include <sys/xattr.h>

#include "../BaseTestCase.h"
#include "../../wrapper/workload.h"
#include "../../wrapper/actions.h"

using tests::DataTestResult;
using wrapper::WriteData;
using wrapper::WriteDataMmap;
using wrapper::Checkpoint;
using std::string;

#define TEST_FILE_PERMS  ((mode_t) (S_IRWXU | S_IRWXG | S_IRWXO))

    namespace tests {
        
        
        class testName: public BaseTestCase {
            
            public:
            
            virtual int setup() override {
                
				test_path = mnt_dir_ ;
				A_path = mnt_dir_ + "/A";
				AC_path = mnt_dir_ + "/A/C";
				B_path = mnt_dir_ + "/B";
				foo_path = mnt_dir_ + "/foo";
				bar_path = mnt_dir_ + "/bar";
				Afoo_path = mnt_dir_ + "/A/foo";
				Abar_path = mnt_dir_ + "/A/bar";
				Bfoo_path = mnt_dir_ + "/B/foo";
				Bbar_path = mnt_dir_ + "/B/bar";
				ACfoo_path = mnt_dir_ + "/A/C/foo";
				ACbar_path = mnt_dir_ + "/A/C/bar";
                return 0;
            }
            
            virtual int run( int checkpoint ) override {
                
				test_path = mnt_dir_ ;
				A_path =  mnt_dir_ + "/A";
				AC_path =  mnt_dir_ + "/A/C";
				B_path =  mnt_dir_ + "/B";
				foo_path =  mnt_dir_ + "/foo";
				bar_path =  mnt_dir_ + "/bar";
				Afoo_path =  mnt_dir_ + "/A/foo";
				Abar_path =  mnt_dir_ + "/A/bar";
				Bfoo_path =  mnt_dir_ + "/B/foo";
				Bbar_path =  mnt_dir_ + "/B/bar";
				ACfoo_path =  mnt_dir_ + "/A/C/foo";
				ACbar_path =  mnt_dir_ + "/A/C/bar";
				int local_checkpoint = 0 ;

				int fd_foo = cm_->CmOpen(foo_path.c_str() , O_RDWR|O_CREAT , 0777); 
				if ( fd_foo < 0 ) { 
					cm_->CmClose( fd_foo); 
					return errno;
				}


				if ( fsetxattr( fd_foo, "user.xattr1", "val1 ", 4, 0 ) < 0){ 
					return errno;
				}


				if ( removexattr(foo_path.c_str() , "user.xattr1") < 0){ 
					return errno;
				}


				cm_->CmSync(); 


				if ( cm_->CmCheckpoint() < 0){ 
					return -1;
				}
				local_checkpoint += 1; 
				if (local_checkpoint == checkpoint) { 
					return 1;
				}


				if ( cm_->CmClose ( fd_foo) < 0){ 
					return errno;
				}

                return 0;
            }
            
            virtual int check_test( unsigned int last_checkpoint, DataTestResult *test_result) override {
                
				test_path = mnt_dir_ ;
				A_path =  mnt_dir_ + "/A";
				AC_path =  mnt_dir_ + "/A/C";
				B_path =  mnt_dir_ + "/B";
				foo_path =  mnt_dir_ + "/foo";
				bar_path =  mnt_dir_ + "/bar";
				Afoo_path =  mnt_dir_ + "/A/foo";
				Abar_path =  mnt_dir_ + "/A/bar";
				Bfoo_path =  mnt_dir_ + "/B/foo";
				Bbar_path =  mnt_dir_ + "/B/bar";
				ACfoo_path =  mnt_dir_ + "/A/C/foo";
				ACbar_path =  mnt_dir_ + "/A/C/bar";
                return 0;
            }
                       
            private:
                       
			 string test_path; 
			 string A_path; 
			 string AC_path; 
			 string B_path; 
			 string foo_path; 
			 string bar_path; 
			 string Afoo_path; 
			 string Abar_path; 
			 string Bfoo_path; 
			 string Bbar_path; 
			 string ACfoo_path; 
			 string ACbar_path; 
                       
            };
                       
    }  // namespace tests
                       
   extern "C" tests::BaseTestCase *test_case_get_instance() {
       return new tests::testName;
   }
                       
   extern "C" void test_case_delete_instance(tests::BaseTestCase *tc) {
       delete tc;
   }