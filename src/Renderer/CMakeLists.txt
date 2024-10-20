set(SOURCES
  StandardRenderer.cpp
)

add_library(Renderer ${SOURCES})
target_include_directories(Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
