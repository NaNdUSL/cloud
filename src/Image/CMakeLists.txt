set(SOURCES
  ImagePPM.cpp
  ImageJPG.cpp
  ImagePFM.cpp
  ImageEXR.cpp
)


add_library(Image ${SOURCES})

target_include_directories(Image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
