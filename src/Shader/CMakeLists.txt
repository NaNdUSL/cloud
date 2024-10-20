set(SOURCES
  AmbientShader.cpp
  WhittedShader.cpp
  DistributedShader.cpp
  PathTracerShader.cpp
)

add_library(Shader ${SOURCES})
target_include_directories(Shader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
