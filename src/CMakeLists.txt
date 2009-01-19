# Compile all libraries
ADD_LIBRARY(libdata data.h data.cpp)
ADD_LIBRARY(libinput input.h input.cpp)
ADD_LIBRARY(liboutput output.h output.cpp)
ADD_LIBRARY(libfile file.h file.cpp)

# Include wxWidgets
SET(wxWidgets_USE_LIBS base core) 
FIND_PACKAGE(wxWidgets REQUIRED) 
INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})

# Define the main inkpad executable, and what it should be linked too
ADD_EXECUTABLE(inkpad WIN32 main)
TARGET_LINK_LIBRARIES(inkpad libdata)
TARGET_LINK_LIBRARIES(inkpad libinput)
TARGET_LINK_LIBRARIES(inkpad liboutput)
TARGET_LINK_LIBRARIES(inkpad libfile)
TARGET_LINK_LIBRARIES(inkpad ${wxWidgets_LIBRARIES})

# Use uSTD?
IF(WITH_USTD)
	MESSAGE ("** Using uSTD data structures")
	ADD_DEFINITIONS(-DWITH_USTD)
	ADD_LIBRARY(libustd ustd.h ustd.cpp)
	TARGET_LINK_LIBRARIES(inkpad libustd)
ENDIF(WITH_USTD)
# Render engines?
IF(RENDER_CAIRO)
	MESSAGE ("** Including Cairo render")
	ADD_DEFINITIONS(-DRENDER_CAIRO)
ENDIF(RENDER_CAIRO)
IF(RENDER_WXWIDGETS)
	MESSAGE ("** Including wxWidgets render")
	ADD_DEFINITIONS(-DRENDER_WXWIDGETS)
ENDIF(RENDER_WXWIDGETS)
