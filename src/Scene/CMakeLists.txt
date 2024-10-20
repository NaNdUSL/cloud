set(SOURCES
  scene.cpp
)

add_library(Scene ${SOURCES})
target_include_directories(Scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
