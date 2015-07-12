# Find PythonQt
#
# Sets PYTHONQT_FOUND, PYTHONQT_INCLUDE_DIR, PYTHONQT_LIBRARY, PYTHONQT_LIBRARIES
#

# Python is required
find_package(PythonLibs)
if(NOT PYTHONLIBS_FOUND)
  message(FATAL_ERROR "error: Python is required to build PythonQt")
endif()
if(NOT EXISTS "${PYTHONQT_ROOT_DIR}")
  find_path(PYTHONQT_ROOT_DIR src/PythonQt.h DOC "PythonQT path.")
endif()
find_path(PYTHONQT_INCLUDE_DIR PythonQt.h
  PATHS "${PYTHONQT_ROOT_DIR}/include/PythonQt"
        "${PYTHONQT_ROOT_DIR}/src"
  DOC "Path to the PythonQt include directory")
#find_library(PYTHONQT_LIBRARY_RELEASE libPythonQt.a PATHS "${PYTHONQT_ROOT_DIR} ${PYTHONQT_ROOT_DIR}/lib" DOC "The PythonQt library.")
#find_library(PYTHONQT_QTALL_LIBRARY_RELEASE PythonQt_QtAll PATHS "${PYTHONQT_ROOT_DIR}/lib" DOC "The PythonQt library.")
find_library(PYTHONQT_LIBRARY libPythonQt.a PATHS ${PYTHONQT_ROOT_DIR} ${PYTHONQT_ROOT_DIR}/lib DOC "The PythonQt library.")
#find_library(PYTHONQT_QTALL_LIBRARY_DEBUG libPythonQt_QtAll.so PATHS "${PYTHONQT_ROOT_DIR}/lib" DOC "The PythonQt library.")
#set(PYTHONQT_LIBRARY)
#if(PYTHONQT_LIBRARY_RELEASE)
#  list(APPEND PYTHONQT_LIBRARY optimized ${PYTHONQT_LIBRARY_RELEASE})
#endif()
#if(PYTHONQT_LIBRARY_DEBUG)
#  list(APPEND PYTHONQT_LIBRARY debug ${PYTHONQT_LIBRARY_DEBUG})
#endif()

#set(PYTHONQT_QTALL_LIBRARY)
#if(PYTHONQT_QTALL_LIBRARY_RELEASE)
#  list(APPEND PYTHONQT_QTALL_LIBRARY optimized ${PYTHONQT_QTALL_LIBRARY_RELEASE})
#endif()
#if(PYTHONQT_QTALL_LIBRARY_DEBUG)
#  list(APPEND PYTHONQT_QTALL_LIBRARY debug ${PYTHONQT_QTALL_LIBRARY_DEBUG})
#endif()

mark_as_advanced(PYTHONQT_ROOT_DIR)
mark_as_advanced(PYTHONQT_INCLUDE_DIR)
mark_as_advanced(PYTHONQT_LIBRARY_RELEASE)
mark_as_advanced(PYTHONQT_LIBRARY_DEBUG)
#mark_as_advanced(PYTHONQT_QTALL_LIBRARY_RELEASE)
#mark_as_advanced(PYTHONQT_QTALL_LIBRARY_DEBUG)

# On linux, also find libutil
#if(UNIX AND NOT APPLE)
#  find_library(PYTHONQT_LIBUTIL util)
#  mark_as_advanced(PYTHONQT_LIBUTIL)
#endif()

# All upper case _FOUND variable is maintained for backwards compatibility.
set(PYTHONQT_FOUND 0)
set(PythonQt_FOUND 0)
if(PYTHONQT_INCLUDE_DIR AND PYTHONQT_LIBRARY)
  # Currently CMake'ified PythonQt only supports building against a python Release build. 
  # This applies independently of CTK build type (Release, Debug, ...)
  add_definitions(-DPYTHONQT_USE_RELEASE_PYTHON_FALLBACK)
  set(PYTHONQT_FOUND 1)
  set(PythonQt_FOUND ${PYTHONQT_FOUND})
  set(PYTHONQT_LIBRARIES ${PYTHONQT_LIBRARY} ${PYTHONQT_LIBUTIL})
endif()
