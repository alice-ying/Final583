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
CMAKE_SOURCE_DIR = /home/acying/Final583

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/acying/Final583/build

# Utility rule file for install-LLVMPJT-stripped.

# Include the progress variables for this target.
include gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/progress.make

gather_data/CMakeFiles/install-LLVMPJT-stripped:
	cd /home/acying/Final583/build/gather_data && /usr/bin/cmake -DCMAKE_INSTALL_COMPONENT="LLVMPJT" -DCMAKE_INSTALL_DO_STRIP=1 -P /home/acying/Final583/build/cmake_install.cmake

install-LLVMPJT-stripped: gather_data/CMakeFiles/install-LLVMPJT-stripped
install-LLVMPJT-stripped: gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/build.make

.PHONY : install-LLVMPJT-stripped

# Rule to build all files generated by this target.
gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/build: install-LLVMPJT-stripped

.PHONY : gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/build

gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/clean:
	cd /home/acying/Final583/build/gather_data && $(CMAKE_COMMAND) -P CMakeFiles/install-LLVMPJT-stripped.dir/cmake_clean.cmake
.PHONY : gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/clean

gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/depend:
	cd /home/acying/Final583/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/acying/Final583 /home/acying/Final583/gather_data /home/acying/Final583/build /home/acying/Final583/build/gather_data /home/acying/Final583/build/gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gather_data/CMakeFiles/install-LLVMPJT-stripped.dir/depend

