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
CMAKE_SOURCE_DIR = /home/orange/graDesign/endRL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/orange/graDesign/endRL/build

# Include any dependencies generated for this target.
include script/CMakeFiles/readOut.dir/depend.make

# Include the progress variables for this target.
include script/CMakeFiles/readOut.dir/progress.make

# Include the compile flags for this target's objects.
include script/CMakeFiles/readOut.dir/flags.make

script/CMakeFiles/readOut.dir/main.cpp.o: script/CMakeFiles/readOut.dir/flags.make
script/CMakeFiles/readOut.dir/main.cpp.o: ../script/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/orange/graDesign/endRL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object script/CMakeFiles/readOut.dir/main.cpp.o"
	cd /home/orange/graDesign/endRL/build/script && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/readOut.dir/main.cpp.o -c /home/orange/graDesign/endRL/script/main.cpp

script/CMakeFiles/readOut.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/readOut.dir/main.cpp.i"
	cd /home/orange/graDesign/endRL/build/script && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/orange/graDesign/endRL/script/main.cpp > CMakeFiles/readOut.dir/main.cpp.i

script/CMakeFiles/readOut.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/readOut.dir/main.cpp.s"
	cd /home/orange/graDesign/endRL/build/script && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/orange/graDesign/endRL/script/main.cpp -o CMakeFiles/readOut.dir/main.cpp.s

script/CMakeFiles/readOut.dir/readerDemo.cpp.o: script/CMakeFiles/readOut.dir/flags.make
script/CMakeFiles/readOut.dir/readerDemo.cpp.o: ../script/readerDemo.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/orange/graDesign/endRL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object script/CMakeFiles/readOut.dir/readerDemo.cpp.o"
	cd /home/orange/graDesign/endRL/build/script && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/readOut.dir/readerDemo.cpp.o -c /home/orange/graDesign/endRL/script/readerDemo.cpp

script/CMakeFiles/readOut.dir/readerDemo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/readOut.dir/readerDemo.cpp.i"
	cd /home/orange/graDesign/endRL/build/script && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/orange/graDesign/endRL/script/readerDemo.cpp > CMakeFiles/readOut.dir/readerDemo.cpp.i

script/CMakeFiles/readOut.dir/readerDemo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/readOut.dir/readerDemo.cpp.s"
	cd /home/orange/graDesign/endRL/build/script && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/orange/graDesign/endRL/script/readerDemo.cpp -o CMakeFiles/readOut.dir/readerDemo.cpp.s

# Object files for target readOut
readOut_OBJECTS = \
"CMakeFiles/readOut.dir/main.cpp.o" \
"CMakeFiles/readOut.dir/readerDemo.cpp.o"

# External object files for target readOut
readOut_EXTERNAL_OBJECTS =

../bin/readOut: script/CMakeFiles/readOut.dir/main.cpp.o
../bin/readOut: script/CMakeFiles/readOut.dir/readerDemo.cpp.o
../bin/readOut: script/CMakeFiles/readOut.dir/build.make
../bin/readOut: script/CMakeFiles/readOut.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/orange/graDesign/endRL/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../bin/readOut"
	cd /home/orange/graDesign/endRL/build/script && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/readOut.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
script/CMakeFiles/readOut.dir/build: ../bin/readOut

.PHONY : script/CMakeFiles/readOut.dir/build

script/CMakeFiles/readOut.dir/clean:
	cd /home/orange/graDesign/endRL/build/script && $(CMAKE_COMMAND) -P CMakeFiles/readOut.dir/cmake_clean.cmake
.PHONY : script/CMakeFiles/readOut.dir/clean

script/CMakeFiles/readOut.dir/depend:
	cd /home/orange/graDesign/endRL/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/orange/graDesign/endRL /home/orange/graDesign/endRL/script /home/orange/graDesign/endRL/build /home/orange/graDesign/endRL/build/script /home/orange/graDesign/endRL/build/script/CMakeFiles/readOut.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : script/CMakeFiles/readOut.dir/depend

