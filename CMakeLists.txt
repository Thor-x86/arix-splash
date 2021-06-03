cmake_minimum_required(VERSION 3.0)
project(arix-splash VERSION 1.0.0)

# Dependency: GIFLIB
file(GLOB_RECURSE GIFLIB_SOURCE_PATH "lib/giflib/*.c")
add_library(
  ${PROJECT_NAME}-GIFLIB STATIC
  ${GIFLIB_SOURCE_PATH}
)
include_directories(
  ${PROJECT_NAME}-GIFLIB
  "lib/giflib/"
)

# Main Program
file(GLOB_RECURSE MAIN_SOURCE_PATH "src/*.c")
add_executable(
  ${PROJECT_NAME}
  ${MAIN_SOURCE_PATH}
)

# Dependencies Linking
target_link_libraries(
  ${PROJECT_NAME}
  ${PROJECT_NAME}-GIFLIB
  pthread
)

# Dependencies Include
include_directories(
  ${PROJECT_NAME}
  "lib/giflib/"
)
