# CMakeList.txt : CMake project for DWNOTools, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

project ("DWNOTools")

include(cmake/CPM.cmake)

# --- Dependencies ---
CPMAddPackage(
  NAME Boost
  VERSION 1.80.0
  GITHUB_REPOSITORY "boostorg/boost"
  GIT_TAG "boost-1.80.0"
)

CPMAddPackage(
  NAME AriaCsvParser
  VERSION 0.0.1
  GITHUB_REPOSITORY "AriaFallah/csv-parser"
  GIT_TAG "c432072c208303e04a9d6b43ecd83d7d568d2981"
)

# --- Building ---
add_executable (DWNOTools "src/DWNOTools.cpp" "src/CSVBExporter.cpp" "src/utils.cpp" "src/CSVB.cpp" "src/CSVBImporter.cpp")

target_link_libraries(DWNOTools PRIVATE Boost::json Boost::algorithm Boost::program_options AriaCsvParser)

set_property(TARGET DWNOTools PROPERTY CXX_STANDARD 20)

# --- Install ---
install(TARGETS DWNOTools DESTINATION DWNOTools)
install(FILES LICENSE THIRD-PARTY-NOTICE DESTINATION DWNOTools/license)
install(FILES README.md DESTINATION DWNOTools)
install(DIRECTORY structures/ DESTINATION DWNOTools/structures)