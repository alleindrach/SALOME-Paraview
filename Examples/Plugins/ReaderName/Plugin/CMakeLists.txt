paraview_add_plugin(ReaderName
  VERSION "1.0"
  REQUIRED_ON_CLIENT
  REQUIRED_ON_SERVER
  MODULES PNGReader
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/MyReader/vtk.module"
  SERVER_MANAGER_XML ReaderName.xml)

option(BUILD_TESTING "Build Plugin Testing" OFF)
if (BUILD_TESTING AND BUILD_SHARED_LIBS)
  add_subdirectory(Testing)
endif()
