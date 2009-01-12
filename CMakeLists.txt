# We require a sufficiently new version of CMake (for wxWidgets detection)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Project name
PROJECT(Inkpad)

# Type of build ("debug" or "release")
set(CMAKE_BUILD_TYPE debug)

# Use the experimental set of STD replacement data structures?
set(INKPAD_USTD false)

# Add individual subdirectories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)

