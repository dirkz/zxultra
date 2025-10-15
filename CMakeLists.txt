# CMakeList.txt : CMake project for zxultra, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("zxultra-main")

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(_UNICODE UNICODE)

# Add source to this project's executable.

add_executable (${CMAKE_PROJECT_NAME} WIN32 "main.cpp" "BaseWindow.h")

add_subdirectory(zxultra)
add_subdirectory(shaders)

add_dependencies(${CMAKE_PROJECT_NAME} shaders)

target_link_libraries(${CMAKE_PROJECT_NAME} zxultra zxultra-core)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
