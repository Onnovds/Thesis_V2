cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(satellite0)

find_package(Geant4 REQUIRED ui_all vis_all)

include(${Geant4_USE_FILE})

file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

file(GLOB MACRO_FILES
	"*.mac"
	)
file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

add_executable(satellite_test satellite_test.cc ${sources} ${headers})
target_link_libraries(satellite_test ${Geant4_LIBRARIES})

add_custom_target(satellite0 DEPENDS satellite_test)



