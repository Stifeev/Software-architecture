# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/stud/arch_labs/LR1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stud/arch_labs/LR1/build

# Include any dependencies generated for this target.
include CMakeFiles/tests.exe.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tests.exe.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tests.exe.dir/flags.make

CMakeFiles/tests.exe.dir/tests.cpp.o: CMakeFiles/tests.exe.dir/flags.make
CMakeFiles/tests.exe.dir/tests.cpp.o: ../tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stud/arch_labs/LR1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tests.exe.dir/tests.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tests.exe.dir/tests.cpp.o -c /home/stud/arch_labs/LR1/tests.cpp

CMakeFiles/tests.exe.dir/tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tests.exe.dir/tests.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stud/arch_labs/LR1/tests.cpp > CMakeFiles/tests.exe.dir/tests.cpp.i

CMakeFiles/tests.exe.dir/tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tests.exe.dir/tests.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stud/arch_labs/LR1/tests.cpp -o CMakeFiles/tests.exe.dir/tests.cpp.s

# Object files for target tests.exe
tests_exe_OBJECTS = \
"CMakeFiles/tests.exe.dir/tests.cpp.o"

# External object files for target tests.exe
tests_exe_EXTERNAL_OBJECTS =

tests.exe: CMakeFiles/tests.exe.dir/tests.cpp.o
tests.exe: CMakeFiles/tests.exe.dir/build.make
tests.exe: /usr/local/lib/libPocoNetSSL.so.81
tests.exe: /usr/lib/x86_64-linux-gnu/libgtest.a
tests.exe: /usr/lib/x86_64-linux-gnu/libz.so
tests.exe: /usr/local/lib/libPocoUtil.so.81
tests.exe: /usr/local/lib/libPocoXML.so.81
tests.exe: /usr/local/lib/libPocoJSON.so.81
tests.exe: /usr/local/lib/libPocoNet.so.81
tests.exe: /usr/local/lib/libPocoCrypto.so.81
tests.exe: /usr/local/lib/libPocoFoundation.so.81
tests.exe: /usr/lib/x86_64-linux-gnu/libssl.so
tests.exe: /usr/lib/x86_64-linux-gnu/libcrypto.so
tests.exe: CMakeFiles/tests.exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/stud/arch_labs/LR1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable tests.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tests.exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tests.exe.dir/build: tests.exe

.PHONY : CMakeFiles/tests.exe.dir/build

CMakeFiles/tests.exe.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tests.exe.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tests.exe.dir/clean

CMakeFiles/tests.exe.dir/depend:
	cd /home/stud/arch_labs/LR1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stud/arch_labs/LR1 /home/stud/arch_labs/LR1 /home/stud/arch_labs/LR1/build /home/stud/arch_labs/LR1/build /home/stud/arch_labs/LR1/build/CMakeFiles/tests.exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tests.exe.dir/depend

