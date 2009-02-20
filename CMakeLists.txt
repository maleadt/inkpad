# We require a sufficiently new version of CMake (for wxWidgets detection)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Project name
PROJECT(Inkpad)

# Type of build ("debug" or "release")
set(CMAKE_BUILD_TYPE debug)

# Experimental options
set(WITH_USTD false)
set(WITH_OPENMP false)

# Which render engines to use
set(RENDER_CAIRO true)
set(RENDER_WXWIDGETS true)

# Add individual subdirectories
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)

