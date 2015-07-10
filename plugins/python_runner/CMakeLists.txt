INCLUDE(BuildPlugin)

BUILD_PLUGIN(python_runner python_runner.cpp python_runner.h MOCFILES python_runner.h  EMBEDDED_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
