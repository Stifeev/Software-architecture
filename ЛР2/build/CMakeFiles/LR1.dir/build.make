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
include CMakeFiles/LR1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/LR1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LR1.dir/flags.make

CMakeFiles/LR1.dir/main.cpp.o: CMakeFiles/LR1.dir/flags.make
CMakeFiles/LR1.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stud/arch_labs/LR1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LR1.dir/main.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/LR1.dir/main.cpp.o -c /home/stud/arch_labs/LR1/main.cpp

CMakeFiles/LR1.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LR1.dir/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stud/arch_labs/LR1/main.cpp > CMakeFiles/LR1.dir/main.cpp.i

CMakeFiles/LR1.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LR1.dir/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stud/arch_labs/LR1/main.cpp -o CMakeFiles/LR1.dir/main.cpp.s

# Object files for target LR1
LR1_OBJECTS = \
"CMakeFiles/LR1.dir/main.cpp.o"

# External object files for target LR1
LR1_EXTERNAL_OBJECTS =

LR1: CMakeFiles/LR1.dir/main.cpp.o
LR1: CMakeFiles/LR1.dir/build.make
LR1: /usr/local/lib/libPocoNetSSL.so.81
LR1: /usr/lib/x86_64-linux-gnu/libz.so
LR1: /usr/local/lib/libPocoUtil.so.81
LR1: /usr/local/lib/libPocoXML.so.81
LR1: /usr/local/lib/libPocoJSON.so.81
LR1: /usr/local/lib/libPocoNet.so.81
LR1: /usr/local/lib/libPocoCrypto.so.81
LR1: /usr/local/lib/libPocoFoundation.so.81
LR1: /usr/lib/x86_64-linux-gnu/libssl.so
LR1: /usr/lib/x86_64-linux-gnu/libcrypto.so
LR1: CMakeFiles/LR1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/stud/arch_labs/LR1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable LR1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LR1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LR1.dir/build: LR1

.PHONY : CMakeFiles/LR1.dir/build

CMakeFiles/LR1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LR1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LR1.dir/clean

CMakeFiles/LR1.dir/depend:
	cd /home/stud/arch_labs/LR1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stud/arch_labs/LR1 /home/stud/arch_labs/LR1 /home/stud/arch_labs/LR1/build /home/stud/arch_labs/LR1/build /home/stud/arch_labs/LR1/build/CMakeFiles/LR1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LR1.dir/depend

