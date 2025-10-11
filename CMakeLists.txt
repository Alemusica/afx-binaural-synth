
cmake_minimum_required(VERSION 3.23)
project(afx_binaural_synth LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Max SDK path (set via env or -DMAX_SDK_DIR=...)
if(NOT DEFINED MAX_SDK_DIR)
  if(DEFINED ENV{MAX_SDK_DIR})
    set(MAX_SDK_DIR $ENV{MAX_SDK_DIR})
  else()
    message(FATAL_ERROR "MAX_SDK_DIR non definito. Esporta MAX_SDK_DIR o passa -DMAX_SDK_DIR=/path/al/max-sdk")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(MaxSDK)

file(GLOB SRC_MAX
  src/max/*.cpp
)
file(GLOB SRC_ENGINE
  src/engine/*.cpp
)
file(GLOB SRC_DSP_HEADERS
  src/dsp/*.hpp
  src/util/*.hpp
)

set(METAL_SRC metal/binaural_kernels.metal)
set(METAL_LIB ${CMAKE_BINARY_DIR}/binaural.metallib)

# Compila il metallib (custom command)
add_custom_command(OUTPUT ${METAL_LIB}
  COMMAND xcrun -sdk macosx metal ${METAL_SRC} -o ${CMAKE_BINARY_DIR}/binaural.air
  COMMAND xcrun -sdk macosx metallib ${CMAKE_BINARY_DIR}/binaural.air -o ${METAL_LIB}
  DEPENDS ${METAL_SRC}
  COMMENT "Compiling Metal kernels to ${METAL_LIB}"
)

add_custom_target(metal_kernels ALL DEPENDS ${METAL_LIB})

add_library(afx.binaural~ MODULE
  ${SRC_MAX}
  ${SRC_ENGINE}
  ${SRC_DSP_HEADERS}
  ${METAL_LIB}
)

# Forza estensione bundle .mxo per Max
set_target_properties(afx.binaural~ PROPERTIES
  BUNDLE TRUE
  BUNDLE_EXTENSION "mxo"
  OUTPUT_NAME "afx.binaural~"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
)

add_dependencies(afx.binaural~ metal_kernels)

target_include_directories(afx.binaural~ PRIVATE
  ${MAX_SDK_INCLUDE_DIRS}
  src
)

target_link_libraries(afx.binaural~
  ${MAX_SDK_LIBS}
  "-framework Metal"
  "-framework MetalPerformanceShaders"
  "-framework Foundation"
  "-framework Accelerate"
)

# Copia il metallib dentro al bundle .mxo/Contents/Resources
add_custom_command(TARGET afx.binaural~ POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:afx.binaural~>/Contents/Resources"
  COMMAND ${CMAKE_COMMAND} -E copy ${METAL_LIB} "$<TARGET_FILE_DIR:afx.binaural~>/Contents/Resources/binaural.metallib"
)
