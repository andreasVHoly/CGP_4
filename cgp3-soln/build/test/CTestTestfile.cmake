# CMake generated Testfile for 
# Source directory: /home/user/Honours/CGP/Ass4/cgp3-soln/test
# Build directory: /home/user/Honours/CGP/Ass4/cgp3-soln/build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(smoketest "/home/user/Honours/CGP/Ass4/cgp3-soln/build/test/tilertest" "-v" "--test=commit")
SET_TESTS_PROPERTIES(smoketest PROPERTIES  WORKING_DIRECTORY "/home/user/Honours/CGP/Ass4/cgp3-soln/build")
