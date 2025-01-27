//
//  AmbientShader.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef AmbientShader_hpp
#define AmbientShader_hpp

#include "shader.hpp"

class AmbientShader: public Shader {
    RGB background;
    Scene *scene;
public:
    AmbientShader (Scene *scene, RGB bg): background(bg), scene(scene), Shader(scene) {}
    RGB shade (bool intersected, Intersection isect, int depth);
};

#endif /* AmbientShader_hpp */
