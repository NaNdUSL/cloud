project(VI)

add_subdirectory(Camera)
add_subdirectory(Image)
add_subdirectory(Primitive)
add_subdirectory(Renderer)
add_subdirectory(Scene)
add_subdirectory(Shader)
add_subdirectory(utils)

add_executable(VI main.cpp)

find_package(JPEG REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(jsoncpp REQUIRED)

target_link_libraries(VI stdc++fs)
target_link_libraries(Image JPEG::JPEG)
target_link_libraries(Image OpenEXR::OpenEXR)
target_link_libraries(VI Camera Image Primitive Renderer Scene Shader jsoncpp_lib)
# falta o utils
