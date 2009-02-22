# Compile all libraries
ADD_LIBRARY(exception exception.h exception.cpp)
ADD_LIBRARY(generic generic.h generic.cpp)
ADD_LIBRARY(threading threading.h threading.cpp)
ADD_LIBRARY(data data.h data.cpp)
ADD_LIBRARY(input input.h input.cpp)
ADD_LIBRARY(output output.h output.cpp)
ADD_LIBRARY(file file.h file.cpp)
ADD_LIBRARY(render render.h render.cpp)

# Include wxWidgets
SET(wxWidgets_USE_LIBS base core) 
FIND_PACKAGE(wxWidgets REQUIRED) 
INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})

# Include Boost
FIND_PACKAGE(Boost)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# Define the main inkpad executable, and what it should be linked too
ADD_EXECUTABLE(inkpad WIN32 main)
TARGET_LINK_LIBRARIES(inkpad exception)
TARGET_LINK_LIBRARIES(inkpad generic)
TARGET_LINK_LIBRARIES(inkpad threading)
TARGET_LINK_LIBRARIES(inkpad data)
TARGET_LINK_LIBRARIES(inkpad input)
TARGET_LINK_LIBRARIES(inkpad output)
TARGET_LINK_LIBRARIES(inkpad file)
TARGET_LINK_LIBRARIES(inkpad render)
TARGET_LINK_LIBRARIES(inkpad ${wxWidgets_LIBRARIES})

# Use uSTD?
IF (WITH_USTD)
	MESSAGE("** Using uSTD data structures")
	ADD_DEFINITIONS(-DWITH_USTD)
	ADD_LIBRARY(libustd ustd.h ustd.cpp)
	TARGET_LINK_LIBRARIES(inkpad libustd)
ENDIF(WITH_USTD)

# Use OpenMP?
INCLUDE(CheckCCompilerFlag)
IF (WITH_OPENMP)
	CHECK_C_COMPILER_FLAG(-fopenmp HAVE_OPENMP)
	IF (HAVE_OPENMP)
		MESSAGE("** Using OpenMP multithreading")
		ADD_DEFINITIONS(-DWITH_OPENMP)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -openmp")
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")
	ELSE (HAVE_OPENMP)
		MESSAGE("!! Compiler doesn't support OpenMP multithreading, disabling multithreading")
	ENDIF (HAVE_OPENMP)
ENDIF (WITH_OPENMP)

# Render engines?
IF (RENDER_CAIRO)
	MESSAGE("** Building Cairo render")
	ADD_DEFINITIONS(-DRENDER_CAIRO)
ENDIF (RENDER_CAIRO)
IF (RENDER_WXWIDGETS)
	MESSAGE("** Building wxWidgets render")
	ADD_DEFINITIONS(-DRENDER_WXWIDGETS)
ENDIF (RENDER_WXWIDGETS)
