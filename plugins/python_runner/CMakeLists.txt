INCLUDE(BuildPlugin)

BUILD_PLUGIN(python_runner python_runner.cpp python_runner.h python_runner_description.cpp python_runner_description.h python_runner_port_dialog.cpp python_runner_port_dialog.h MOCFILES python_runner.h python_runner_description.h python_runner_port_dialog.h EMBEDDED_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
