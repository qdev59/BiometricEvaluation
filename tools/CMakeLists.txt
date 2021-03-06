#
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility  whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#
# The Biometric Evaluation Framework must be either system-installed  or
# locally built in the common/src/libbiomeval directory, prior to building
# the tools which are linked against the static framework library by default.
#
# To use CMake to create Biometric Evaluation Framework Tools:
#   mkdir build; cd build
#   cmake ..
#   make
#
# The executable programs will be located in the 'bin' directory.
#

project(biomeval-tools)

cmake_minimum_required(VERSION 2.8.11)

# Sanity check our source directory to make sure that we are not trying to
# generate an in-tree build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed.
CMake would overwrite the make files distributed with the Framework tools.
Please create a directory and run cmake from there, passing the path
to this source directory as the last argument.
This process created the file `CMakeCache.txt' and the directory `CMakeFiles'.
Please delete them.")
endif()

#
# Find The Framework library; prefer the 'locally' built version over the
# system-installed version.
#
set(BEFLIBNAME libbiomeval.a)
set(BEFHDRNAME be_framework.h)
find_library(
  LCACHE ${BEFLIBNAME}
  PATHS "${PROJECT_SOURCE_DIR}/../common/src/libbiomeval/"
  NO_DEFAULT_PATH)

if(${LCACHE} STREQUAL "LCACHE-NOTFOUND")
  find_library(LCACHE ${BEFLIBNAME})
  if(${LCACHE} STREQUAL "LCACHE-NOTFOUND")
    message(FATAL_ERROR "Cannot find ${BEFLIBNAME}.")
  endif()
  # Look for one of the Framework .h files in the system
  find_path(HCACHE ${BEFHDRNAME})
  if(${HCACHE} STREQUAL "HCACHE-NOTFOUND")
    message(FATAL_ERROR "Cannot find ${BEFHDRNAME}.")
  endif()
  message(STATUS "Biometric Evaluation Framework ${BEFHDRNAME} found in ${HCACHE}.")
else(${LCACHE} STREQUAL "LCACHE-NOTFOUND")
  # At this point we assume the .h files are in the local area
  include_directories("${PROJECT_SOURCE_DIR}/../common/src/include")
endif(${LCACHE} STREQUAL "LCACHE-NOTFOUND")
message(STATUS "Biometric Evaluation Framework found in ${LCACHE}.")

link_libraries(jpeg tiff openjp2 crypto png z sqlite3 db)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

#
# For OS-X, add paths where the ports packages are placed.
#
if(CMAKE_HOST_APPLE)
  include_directories(SYSTEM /opt/local/include)
  link_directories(/opt/local/lib)
endif(CMAKE_HOST_APPLE)
link_libraries(${LCACHE})

#
# asegcrop
#
set(SRCDIR "${PROJECT_SOURCE_DIR}/src/asegcrop")
file(GLOB src "${SRCDIR}/*cpp")
add_executable(asegcrop ${src})
target_include_directories(asegcrop PUBLIC "${SRCDIR}")
target_link_libraries(asegcrop -lopencv_core -lopencv_imgproc -lopencv_highgui)

#
# efs2incits
#
set(SRCDIR "${PROJECT_SOURCE_DIR}/src/countRecordsOfSize")
#
file(GLOB src "${SRCDIR}/*cpp")
add_executable(countRecordsOfSize ${src})

#
# rstool
#
set(SRCDIR "${PROJECT_SOURCE_DIR}/src/rstool")
#
#
# Get the major and minor library versions from the VERSION file, with format
# expected to be MAJOR_VERSION=mm and MINOR_VERSION=nn on separate lines.
#
execute_process(COMMAND grep MAJOR_VERSION= ${SRCDIR}/VERSION COMMAND cut -f2 -d= OUTPUT_VARIABLE MAJOR_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND grep MINOR_VERSION= ${SRCDIR}/VERSION COMMAND cut -f2 -d= OUTPUT_VARIABLE MINOR_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

find_package(X11)
file(GLOB src "${SRCDIR}/*cpp")
add_executable(rstool ${src})
target_include_directories(rstool PUBLIC "${SRCDIR}")
target_include_directories(rstool PUBLIC "${X11_INCLUDE_DIR}")
target_link_libraries(rstool "${X11_LIBRARIES}")
target_compile_definitions(rstool PUBLIC -DMAJOR_VERSION -DMINOR_VERSION)

#
# efs2incits
#
set(SRCDIR "${PROJECT_SOURCE_DIR}/src/efs2incits")
#
file(GLOB src "${SRCDIR}/*cpp")
add_executable(efs2incits ${src})
target_include_directories(efs2incits PUBLIC "${SRCDIR}")
