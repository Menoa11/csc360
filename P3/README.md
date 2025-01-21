Rubric

Basic Functionality (Total: 1 Mark, 0.5 Mark each)

Your submission contains a README.txt/README.md that clearly describes which features you have implemented and whether they work correctly or not. (Implemented)

Your submission can be successfully compiled by make without errors. Four executables diskinfo, disklist, diskget, and diskput are produced in the root directory of your repository after make. (Implemented)


diskinfo (Total: 2 Mark)

The output of ./diskinfo test.img follows the exact expected format as specified in Section 3.1. (1 Mark) (Implemented)

The output of ./diskinfo test.img is correct, which will be tested against the provided test image, and one additional test image. (Total: 1 Mark, 2 test cases, 0.5 Mark each) (implemented)



disklist (Total: 4 Mark)

The output of ./disklist test.img /sub_dir follows the exact expected format as specified in Section 3.2. (1 Mark) (implemented)

The output of ./disklist is correct, which will be tested against several test cases, such as ./disklist test.img /, ./disklist test.img /sub_dirA, ./disklist test.img /sub_dirA/sub_dirB, etc. (Total: 3 Mark, 3 test cases, 1 Mark each) (Implemented, Worked for my test cases)



diskget (Total: 4 Mark)

./diskget outputs File not found. if the specified file is not found in the specified directory in the test image. (0.5 Mark) (Implemented)

./diskget copies the specified file from the specified directory in the test image to the current directory in the host operating system. (Total: 2 Mark, 2 test cases, 1 Mark each) (Implemented)

The copied file is identical to the original file in the test image. (1.5 Mark) (Implemented)



diskput (Total: 4 Mark)

./diskput outputs File not found. if the specified file is not found in the host operating system. (0.5 Mark) (Implemented)

./diskput copies the specified file from the host operating system to the specified directory in the test image, which can be verified by ./disklist on the specified directory. (Total: 2 Mark) (Not Implemented)

When using ./diskget to copy the file back to the host operating system, the copied file is identical to the original file in the host operating system. (1 Mark) (Not Implemented)

If the specified target subdirectory does not exist in the file system image when executing ./diskput test.img foo.txt /sub_dir/bar.txt, a new directory /sub_dir should be created in the file system image, and the file foo.txt should be copied into the new directory. (0.5 Mark) (Not Implemented)