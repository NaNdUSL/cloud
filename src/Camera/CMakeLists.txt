set(SOURCES
  perspective.cpp
  environment.cpp
)

add_library(Camera ${SOURCES})
target_include_directories(Camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
