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
CMAKE_SOURCE_DIR = /home/orange/graDesign/coding/RLcoding

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/orange/graDesign/coding/RLcoding/build

# Include any dependencies generated for this target.
include CMakeFiles/binaryRL.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/binaryRL.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/binaryRL.dir/flags.make

CMakeFiles/binaryRL.dir/binaryRL.cpp.o: CMakeFiles/binaryRL.dir/flags.make
CMakeFiles/binaryRL.dir/binaryRL.cpp.o: ../binaryRL.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/orange/graDesign/coding/RLcoding/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/binaryRL.dir/binaryRL.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/binaryRL.dir/binaryRL.cpp.o -c /home/orange/graDesign/coding/RLcoding/binaryRL.cpp

CMakeFiles/binaryRL.dir/binaryRL.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/binaryRL.dir/binaryRL.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/orange/graDesign/coding/RLcoding/binaryRL.cpp > CMakeFiles/binaryRL.dir/binaryRL.cpp.i

CMakeFiles/binaryRL.dir/binaryRL.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/binaryRL.dir/binaryRL.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/orange/graDesign/coding/RLcoding/binaryRL.cpp -o CMakeFiles/binaryRL.dir/binaryRL.cpp.s

CMakeFiles/binaryRL.dir/main.cpp.o: CMakeFiles/binaryRL.dir/flags.make
CMakeFiles/binaryRL.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/orange/graDesign/coding/RLcoding/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/binaryRL.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/binaryRL.dir/main.cpp.o -c /home/orange/graDesign/coding/RLcoding/main.cpp

CMakeFiles/binaryRL.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/binaryRL.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/orange/graDesign/coding/RLcoding/main.cpp > CMakeFiles/binaryRL.dir/main.cpp.i

CMakeFiles/binaryRL.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/binaryRL.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/orange/graDesign/coding/RLcoding/main.cpp -o CMakeFiles/binaryRL.dir/main.cpp.s

# Object files for target binaryRL
binaryRL_OBJECTS = \
"CMakeFiles/binaryRL.dir/binaryRL.cpp.o" \
"CMakeFiles/binaryRL.dir/main.cpp.o"

# External object files for target binaryRL
binaryRL_EXTERNAL_OBJECTS =

../bin/binaryRL: CMakeFiles/binaryRL.dir/binaryRL.cpp.o
../bin/binaryRL: CMakeFiles/binaryRL.dir/main.cpp.o
../bin/binaryRL: CMakeFiles/binaryRL.dir/build.make
../bin/binaryRL: CMakeFiles/binaryRL.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/orange/graDesign/coding/RLcoding/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../bin/binaryRL"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/binaryRL.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/binaryRL.dir/build: ../bin/binaryRL

.PHONY : CMakeFiles/binaryRL.dir/build

CMakeFiles/binaryRL.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/binaryRL.dir/cmake_clean.cmake
.PHONY : CMakeFiles/binaryRL.dir/clean

CMakeFiles/binaryRL.dir/depend:
	cd /home/orange/graDesign/coding/RLcoding/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/orange/graDesign/coding/RLcoding /home/orange/graDesign/coding/RLcoding /home/orange/graDesign/coding/RLcoding/build /home/orange/graDesign/coding/RLcoding/build /home/orange/graDesign/coding/RLcoding/build/CMakeFiles/binaryRL.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/binaryRL.dir/depend
