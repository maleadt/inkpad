# Compile all libraries
ADD_LIBRARY(libustd ustd.h ustd.cpp)
ADD_LIBRARY(libdata data.h data.cpp)
ADD_LIBRARY(libinput input.h input.cpp)
ADD_LIBRARY(liboutput output.h output.cpp)
ADD_LIBRARY(libfile file.h file.cpp)

# Include wxWidgets
SET(wxWidgets_USE_LIBS base core) 
FIND_PACKAGE(wxWidgets REQUIRED) 
IF(wxWidgets_FOUND) 
	INCLUDE(${wxWidgets_USE_FILE}) 
	INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS}) 
	LINK_LIBRARIES(${wxWidgets_LIBRARIES}) 
ENDIF(wxWidgets_FOUND)  

# Define the main inkpad executable, and what it should be linked too
ADD_EXECUTABLE(inkpad main)
TARGET_LINK_LIBRARIES(inkpad libdata)
TARGET_LINK_LIBRARIES(inkpad libinput)
TARGET_LINK_LIBRARIES(inkpad liboutput)
TARGET_LINK_LIBRARIES(inkpad libfile)
