# CMake generated Testfile for 
# Source directory: /home/v/vhland002/Honours/cgpassign4/cgp3-soln/test
# Build directory: /home/v/vhland002/Honours/cgpassign4/cgp3-soln/build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(smoketest "/home/v/vhland002/Honours/cgpassign4/cgp3-soln/build/test/tilertest" "-v" "--test=commit")
set_tests_properties(smoketest PROPERTIES  WORKING_DIRECTORY "/home/v/vhland002/Honours/cgpassign4/cgp3-soln/build")
