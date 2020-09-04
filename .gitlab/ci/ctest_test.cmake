include("${CMAKE_CURRENT_LIST_DIR}/gitlab_ci.cmake")

# Read the files from the build directory.
ctest_read_custom_files("${CTEST_BINARY_DIRECTORY}")

# Pick up from where the configure left off.
ctest_start(APPEND)

include(ProcessorCount)
ProcessorCount(nproc)

# Default to a reasonable test timeout.
set(CTEST_TEST_TIMEOUT 100)

set(test_exclusions
)

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos")
  list(APPEND test_exclusions
    # Unstructured grid volume rendering (paraview/paraview#19130)
    "\.MultiBlockVolumeRendering$"
    "\.UnstructuredVolumeRenderingVectorComponent$")
endif ()

string(REPLACE ";" "|" test_exclusions "${test_exclusions}")
if (test_exclusions)
  set(test_exclusions "(${test_exclusions})")
endif ()

ctest_test(APPEND
  PARALLEL_LEVEL "${nproc}"
  RETURN_VALUE test_result
  EXCLUDE "${test_exclusions}")
ctest_submit(PARTS Test)

if (test_result)
  message(FATAL_ERROR
    "Failed to test")
endif ()
