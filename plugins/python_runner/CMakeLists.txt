INCLUDE(BuildPlugin)
find_package(python)
find_package(PythonQt)
if(PythonQt_FOUND)
  message(INFO "PythonQT found")
else()
  message(FATAL_ERROR "PythonQT is not found")
endif (PythonQT_FOUND)

  INCLUDE_DIRECTORIES(${PYTHONQT_INCLUDE_DIR} ${PYTHON_INCLUDE_DIR})
  LINK_LIBRARIES(${PYTHONQT_LIBRARIES} ${PYTHON_LIBRARIES})
  BUILD_PLUGIN(python_runner python_runner.cpp python_runner.h MOCFILES python_runner.h  EMBEDDED_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
