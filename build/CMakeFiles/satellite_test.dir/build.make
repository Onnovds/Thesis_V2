# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/onno/satellite_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/onno/satellite_test/build

# Include any dependencies generated for this target.
include CMakeFiles/satellite_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/satellite_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/satellite_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/satellite_test.dir/flags.make

CMakeFiles/satellite_test.dir/satellite_test.cc.o: CMakeFiles/satellite_test.dir/flags.make
CMakeFiles/satellite_test.dir/satellite_test.cc.o: ../satellite_test.cc
CMakeFiles/satellite_test.dir/satellite_test.cc.o: CMakeFiles/satellite_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/satellite_test.dir/satellite_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/satellite_test.dir/satellite_test.cc.o -MF CMakeFiles/satellite_test.dir/satellite_test.cc.o.d -o CMakeFiles/satellite_test.dir/satellite_test.cc.o -c /home/onno/satellite_test/satellite_test.cc

CMakeFiles/satellite_test.dir/satellite_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/satellite_test.dir/satellite_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onno/satellite_test/satellite_test.cc > CMakeFiles/satellite_test.dir/satellite_test.cc.i

CMakeFiles/satellite_test.dir/satellite_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/satellite_test.dir/satellite_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onno/satellite_test/satellite_test.cc -o CMakeFiles/satellite_test.dir/satellite_test.cc.s

CMakeFiles/satellite_test.dir/action.cc.o: CMakeFiles/satellite_test.dir/flags.make
CMakeFiles/satellite_test.dir/action.cc.o: ../action.cc
CMakeFiles/satellite_test.dir/action.cc.o: CMakeFiles/satellite_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/satellite_test.dir/action.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/satellite_test.dir/action.cc.o -MF CMakeFiles/satellite_test.dir/action.cc.o.d -o CMakeFiles/satellite_test.dir/action.cc.o -c /home/onno/satellite_test/action.cc

CMakeFiles/satellite_test.dir/action.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/satellite_test.dir/action.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onno/satellite_test/action.cc > CMakeFiles/satellite_test.dir/action.cc.i

CMakeFiles/satellite_test.dir/action.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/satellite_test.dir/action.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onno/satellite_test/action.cc -o CMakeFiles/satellite_test.dir/action.cc.s

CMakeFiles/satellite_test.dir/construction.cc.o: CMakeFiles/satellite_test.dir/flags.make
CMakeFiles/satellite_test.dir/construction.cc.o: ../construction.cc
CMakeFiles/satellite_test.dir/construction.cc.o: CMakeFiles/satellite_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/satellite_test.dir/construction.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/satellite_test.dir/construction.cc.o -MF CMakeFiles/satellite_test.dir/construction.cc.o.d -o CMakeFiles/satellite_test.dir/construction.cc.o -c /home/onno/satellite_test/construction.cc

CMakeFiles/satellite_test.dir/construction.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/satellite_test.dir/construction.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onno/satellite_test/construction.cc > CMakeFiles/satellite_test.dir/construction.cc.i

CMakeFiles/satellite_test.dir/construction.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/satellite_test.dir/construction.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onno/satellite_test/construction.cc -o CMakeFiles/satellite_test.dir/construction.cc.s

CMakeFiles/satellite_test.dir/detector.cc.o: CMakeFiles/satellite_test.dir/flags.make
CMakeFiles/satellite_test.dir/detector.cc.o: ../detector.cc
CMakeFiles/satellite_test.dir/detector.cc.o: CMakeFiles/satellite_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/satellite_test.dir/detector.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/satellite_test.dir/detector.cc.o -MF CMakeFiles/satellite_test.dir/detector.cc.o.d -o CMakeFiles/satellite_test.dir/detector.cc.o -c /home/onno/satellite_test/detector.cc

CMakeFiles/satellite_test.dir/detector.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/satellite_test.dir/detector.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onno/satellite_test/detector.cc > CMakeFiles/satellite_test.dir/detector.cc.i

CMakeFiles/satellite_test.dir/detector.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/satellite_test.dir/detector.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onno/satellite_test/detector.cc -o CMakeFiles/satellite_test.dir/detector.cc.s

CMakeFiles/satellite_test.dir/generator.cc.o: CMakeFiles/satellite_test.dir/flags.make
CMakeFiles/satellite_test.dir/generator.cc.o: ../generator.cc
CMakeFiles/satellite_test.dir/generator.cc.o: CMakeFiles/satellite_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/satellite_test.dir/generator.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/satellite_test.dir/generator.cc.o -MF CMakeFiles/satellite_test.dir/generator.cc.o.d -o CMakeFiles/satellite_test.dir/generator.cc.o -c /home/onno/satellite_test/generator.cc

CMakeFiles/satellite_test.dir/generator.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/satellite_test.dir/generator.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onno/satellite_test/generator.cc > CMakeFiles/satellite_test.dir/generator.cc.i

CMakeFiles/satellite_test.dir/generator.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/satellite_test.dir/generator.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onno/satellite_test/generator.cc -o CMakeFiles/satellite_test.dir/generator.cc.s

CMakeFiles/satellite_test.dir/physics.cc.o: CMakeFiles/satellite_test.dir/flags.make
CMakeFiles/satellite_test.dir/physics.cc.o: ../physics.cc
CMakeFiles/satellite_test.dir/physics.cc.o: CMakeFiles/satellite_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/satellite_test.dir/physics.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/satellite_test.dir/physics.cc.o -MF CMakeFiles/satellite_test.dir/physics.cc.o.d -o CMakeFiles/satellite_test.dir/physics.cc.o -c /home/onno/satellite_test/physics.cc

CMakeFiles/satellite_test.dir/physics.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/satellite_test.dir/physics.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onno/satellite_test/physics.cc > CMakeFiles/satellite_test.dir/physics.cc.i

CMakeFiles/satellite_test.dir/physics.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/satellite_test.dir/physics.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onno/satellite_test/physics.cc -o CMakeFiles/satellite_test.dir/physics.cc.s

CMakeFiles/satellite_test.dir/run.cc.o: CMakeFiles/satellite_test.dir/flags.make
CMakeFiles/satellite_test.dir/run.cc.o: ../run.cc
CMakeFiles/satellite_test.dir/run.cc.o: CMakeFiles/satellite_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/satellite_test.dir/run.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/satellite_test.dir/run.cc.o -MF CMakeFiles/satellite_test.dir/run.cc.o.d -o CMakeFiles/satellite_test.dir/run.cc.o -c /home/onno/satellite_test/run.cc

CMakeFiles/satellite_test.dir/run.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/satellite_test.dir/run.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onno/satellite_test/run.cc > CMakeFiles/satellite_test.dir/run.cc.i

CMakeFiles/satellite_test.dir/run.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/satellite_test.dir/run.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onno/satellite_test/run.cc -o CMakeFiles/satellite_test.dir/run.cc.s

# Object files for target satellite_test
satellite_test_OBJECTS = \
"CMakeFiles/satellite_test.dir/satellite_test.cc.o" \
"CMakeFiles/satellite_test.dir/action.cc.o" \
"CMakeFiles/satellite_test.dir/construction.cc.o" \
"CMakeFiles/satellite_test.dir/detector.cc.o" \
"CMakeFiles/satellite_test.dir/generator.cc.o" \
"CMakeFiles/satellite_test.dir/physics.cc.o" \
"CMakeFiles/satellite_test.dir/run.cc.o"

# External object files for target satellite_test
satellite_test_EXTERNAL_OBJECTS =

satellite_test: CMakeFiles/satellite_test.dir/satellite_test.cc.o
satellite_test: CMakeFiles/satellite_test.dir/action.cc.o
satellite_test: CMakeFiles/satellite_test.dir/construction.cc.o
satellite_test: CMakeFiles/satellite_test.dir/detector.cc.o
satellite_test: CMakeFiles/satellite_test.dir/generator.cc.o
satellite_test: CMakeFiles/satellite_test.dir/physics.cc.o
satellite_test: CMakeFiles/satellite_test.dir/run.cc.o
satellite_test: CMakeFiles/satellite_test.dir/build.make
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4Tree.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4FR.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4GMocren.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4visHepRep.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4RayTracer.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4VRML.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4ToolsSG.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4OpenGL.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4vis_management.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4modeling.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4interfaces.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4mctruth.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4geomtext.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4gdml.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4error_propagation.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4readout.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4physicslists.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4run.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4event.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4tracking.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4parmodels.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4processes.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4digits_hits.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4track.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4particles.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4geometry.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4materials.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4graphics_reps.so
satellite_test: /usr/lib/x86_64-linux-gnu/libGL.so
satellite_test: /usr/lib/x86_64-linux-gnu/libQt5OpenGL.so.5.15.3
satellite_test: /usr/lib/x86_64-linux-gnu/libQt5Widgets.so.5.15.3
satellite_test: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5.15.3
satellite_test: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.15.3
satellite_test: /usr/lib/x86_64-linux-gnu/libXmu.so
satellite_test: /usr/lib/x86_64-linux-gnu/libXext.so
satellite_test: /usr/lib/x86_64-linux-gnu/libXt.so
satellite_test: /usr/lib/x86_64-linux-gnu/libICE.so
satellite_test: /usr/lib/x86_64-linux-gnu/libSM.so
satellite_test: /usr/lib/x86_64-linux-gnu/libX11.so
satellite_test: /usr/lib/x86_64-linux-gnu/libxerces-c.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4analysis.so
satellite_test: /usr/lib/x86_64-linux-gnu/libexpat.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4zlib.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4intercoms.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4global.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4clhep.so
satellite_test: /home/onno/Software/Geant4/geant4-v11.2.1-install/lib/libG4ptl.so.2.3.3
satellite_test: CMakeFiles/satellite_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/onno/satellite_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable satellite_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/satellite_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/satellite_test.dir/build: satellite_test
.PHONY : CMakeFiles/satellite_test.dir/build

CMakeFiles/satellite_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/satellite_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/satellite_test.dir/clean

CMakeFiles/satellite_test.dir/depend:
	cd /home/onno/satellite_test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/onno/satellite_test /home/onno/satellite_test /home/onno/satellite_test/build /home/onno/satellite_test/build /home/onno/satellite_test/build/CMakeFiles/satellite_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/satellite_test.dir/depend

