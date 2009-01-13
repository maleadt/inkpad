# Compile all libraries
ADD_LIBRARY(libdata data.h data.cpp)
ADD_LIBRARY(libinput input.h input.cpp)
ADD_LIBRARY(liboutput output.h output.cpp)
ADD_LIBRARY(libfile file.h file.cpp)

# Include wxWidgets
SET(wxWidgets_USE_LIBS core) 
FIND_PACKAGE(wxWidgets REQUIRED) 
INCLUDE(${wxWidgets_USE_FILE}) 
INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})

# uSTD?


# Define the main inkpad executable, and what it should be linked too
ADD_EXECUTABLE(inkpad main)
TARGET_LINK_LIBRARIES(inkpad libdata)
TARGET_LINK_LIBRARIES(inkpad libinput)
TARGET_LINK_LIBRARIES(inkpad liboutput)
TARGET_LINK_LIBRARIES(inkpad libfile)
TARGET_LINK_LIBRARIES(inkpad ${wxWidgets_LIBRARIES})
IF (WITH_USTD)
	MESSAGE ("** Using uSTD data structures")
	ADD_DEFINITIONS(-DWITH_USTD)
	ADD_LIBRARY(libustd ustd.h ustd.cpp)
	TARGET_LINK_LIBRARIES(inkpad libustd)
ENDIF (WITH_USTD)
