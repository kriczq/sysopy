# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.10.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.10.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kriczq/sysopy/KozaWojciech/cw01/zad2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build

# Include any dependencies generated for this target.
include lib/CMakeFiles/objlib.dir/depend.make

# Include the progress variables for this target.
include lib/CMakeFiles/objlib.dir/progress.make

# Include the compile flags for this target's objects.
include lib/CMakeFiles/objlib.dir/flags.make

lib/CMakeFiles/objlib.dir/lib.c.o: lib/CMakeFiles/objlib.dir/flags.make
lib/CMakeFiles/objlib.dir/lib.c.o: ../lib/lib.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object lib/CMakeFiles/objlib.dir/lib.c.o"
	cd /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build/lib && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/objlib.dir/lib.c.o   -c /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/lib/lib.c

lib/CMakeFiles/objlib.dir/lib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/objlib.dir/lib.c.i"
	cd /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build/lib && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/lib/lib.c > CMakeFiles/objlib.dir/lib.c.i

lib/CMakeFiles/objlib.dir/lib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/objlib.dir/lib.c.s"
	cd /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build/lib && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/lib/lib.c -o CMakeFiles/objlib.dir/lib.c.s

lib/CMakeFiles/objlib.dir/lib.c.o.requires:

.PHONY : lib/CMakeFiles/objlib.dir/lib.c.o.requires

lib/CMakeFiles/objlib.dir/lib.c.o.provides: lib/CMakeFiles/objlib.dir/lib.c.o.requires
	$(MAKE) -f lib/CMakeFiles/objlib.dir/build.make lib/CMakeFiles/objlib.dir/lib.c.o.provides.build
.PHONY : lib/CMakeFiles/objlib.dir/lib.c.o.provides

lib/CMakeFiles/objlib.dir/lib.c.o.provides.build: lib/CMakeFiles/objlib.dir/lib.c.o


objlib: lib/CMakeFiles/objlib.dir/lib.c.o
objlib: lib/CMakeFiles/objlib.dir/build.make

.PHONY : objlib

# Rule to build all files generated by this target.
lib/CMakeFiles/objlib.dir/build: objlib

.PHONY : lib/CMakeFiles/objlib.dir/build

lib/CMakeFiles/objlib.dir/requires: lib/CMakeFiles/objlib.dir/lib.c.o.requires

.PHONY : lib/CMakeFiles/objlib.dir/requires

lib/CMakeFiles/objlib.dir/clean:
	cd /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build/lib && $(CMAKE_COMMAND) -P CMakeFiles/objlib.dir/cmake_clean.cmake
.PHONY : lib/CMakeFiles/objlib.dir/clean

lib/CMakeFiles/objlib.dir/depend:
	cd /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kriczq/sysopy/KozaWojciech/cw01/zad2 /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/lib /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build/lib /Users/kriczq/sysopy/KozaWojciech/cw01/zad2/build/lib/CMakeFiles/objlib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/CMakeFiles/objlib.dir/depend

