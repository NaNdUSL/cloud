//
//  Scene.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <iostream>
#include <filesystem> 
#include <string>
#include <vector>
#include "../../libs/tinyobjloader/tiny_obj_loader.h"
#include "../Primitive/primitive.hpp"
#include "../Light/light.hpp"
#include "../Light/AreaLight.hpp"
#include "../Rays/ray.hpp"
#include "../Rays/intersection.hpp"
#include "../Primitive/BRDF/BRDF.hpp"
#include "../Primitive/BRDF/Phong.hpp"
#include "../Image/ImageJPG.hpp"

class Scene {
    std::vector <BRDF *> BRDFs;
    std::vector <Primitive *> prims;
public:
    int numPrimitives, numLights, numBRDFs;
    std::vector <Light *> lights;
    Scene (): numPrimitives(0), numLights(0), numBRDFs(0) {}
    bool Load (const std::string &fname);
    bool SetLights (void) { return true; };
    std::vector<Light*>& GetLights (void) { return lights; }
    bool trace (Ray r, Intersection *isect);
    void increaseLights();
    void addMaterials(std::vector <tinyobj::material_t> materials);
    void readTextureJpeg(Phong &phong, std::string filename);
    void addBRDF(BRDF * BRDF);
    bool visibility (Ray s, const float maxL);
    void printSummary(void) {
        std::cout << "#primitives = " << numPrimitives << " ; ";
        std::cout << "#lights = " << numLights << " ; ";
        std::cout << "#materials = " << numBRDFs << " ;" << std::endl;
    }
    void printInfo(void);
};

#endif /* Scene_hpp */
