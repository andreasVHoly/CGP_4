# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /opt/local/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts"

# Include any dependencies generated for this target.
include common/CMakeFiles/color2gray.dir/depend.make

# Include the progress variables for this target.
include common/CMakeFiles/color2gray.dir/progress.make

# Include the compile flags for this target's objects.
include common/CMakeFiles/color2gray.dir/flags.make

common/CMakeFiles/color2gray.dir/color2gray.cpp.o: common/CMakeFiles/color2gray.dir/flags.make
common/CMakeFiles/color2gray.dir/color2gray.cpp.o: common/color2gray.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/CMakeFiles" $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object common/CMakeFiles/color2gray.dir/color2gray.cpp.o"
	cd "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common" && /opt/local/bin/gcc   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/color2gray.dir/color2gray.cpp.o -c "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common/color2gray.cpp"

common/CMakeFiles/color2gray.dir/color2gray.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/color2gray.dir/color2gray.cpp.i"
	cd "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common" && /opt/local/bin/gcc  $(CXX_DEFINES) $(CXX_FLAGS) -E "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common/color2gray.cpp" > CMakeFiles/color2gray.dir/color2gray.cpp.i

common/CMakeFiles/color2gray.dir/color2gray.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/color2gray.dir/color2gray.cpp.s"
	cd "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common" && /opt/local/bin/gcc  $(CXX_DEFINES) $(CXX_FLAGS) -S "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common/color2gray.cpp" -o CMakeFiles/color2gray.dir/color2gray.cpp.s

common/CMakeFiles/color2gray.dir/color2gray.cpp.o.requires:
.PHONY : common/CMakeFiles/color2gray.dir/color2gray.cpp.o.requires

common/CMakeFiles/color2gray.dir/color2gray.cpp.o.provides: common/CMakeFiles/color2gray.dir/color2gray.cpp.o.requires
	$(MAKE) -f common/CMakeFiles/color2gray.dir/build.make common/CMakeFiles/color2gray.dir/color2gray.cpp.o.provides.build
.PHONY : common/CMakeFiles/color2gray.dir/color2gray.cpp.o.provides

common/CMakeFiles/color2gray.dir/color2gray.cpp.o.provides.build: common/CMakeFiles/color2gray.dir/color2gray.cpp.o

# Object files for target color2gray
color2gray_OBJECTS = \
"CMakeFiles/color2gray.dir/color2gray.cpp.o"

# External object files for target color2gray
color2gray_EXTERNAL_OBJECTS =

common/color2gray: common/CMakeFiles/color2gray.dir/color2gray.cpp.o
common/color2gray: common/CMakeFiles/color2gray.dir/build.make
common/color2gray: common/libcommon.a
common/color2gray: common/libcommon_rgba.a
common/color2gray: common/libcommon.a
common/color2gray: /opt/local/lib/libboost_serialization-mt.dylib
common/color2gray: common/CMakeFiles/color2gray.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable color2gray"
	cd "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/color2gray.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
common/CMakeFiles/color2gray.dir/build: common/color2gray
.PHONY : common/CMakeFiles/color2gray.dir/build

common/CMakeFiles/color2gray.dir/requires: common/CMakeFiles/color2gray.dir/color2gray.cpp.o.requires
.PHONY : common/CMakeFiles/color2gray.dir/requires

common/CMakeFiles/color2gray.dir/clean:
	cd "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common" && $(CMAKE_COMMAND) -P CMakeFiles/color2gray.dir/cmake_clean.cmake
.PHONY : common/CMakeFiles/color2gray.dir/clean

common/CMakeFiles/color2gray.dir/depend:
	cd "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts" "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common" "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts" "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common" "/Volumes/1Tb Macintosh HD/Users/Jed/Desktop/PROGRAMMING/uts/common/CMakeFiles/color2gray.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : common/CMakeFiles/color2gray.dir/depend

