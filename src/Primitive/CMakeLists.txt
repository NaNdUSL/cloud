set(SOURCES
  BRDF/Phong.cpp
  Geometry/mesh.cpp
)

add_library(Primitive ${SOURCES})
target_include_directories(Primitive PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
