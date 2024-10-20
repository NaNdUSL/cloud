#include <iostream>
#include <filesystem>
#include "CfgReader.hpp"
#include "Camera/perspective.hpp"
#include "Camera/environment.hpp"
#include "Image/ImagePPM.hpp"
#include "Image/ImageJPG.hpp"
#include "Image/ImagePFM.hpp"
#include "Image/ImageEXR.hpp"
#include "Renderer/StandardRenderer.hpp"
#include "Scene/scene.hpp"
#include "Shader/WhittedShader.hpp"
#include "Shader/AmbientShader.hpp"
#include "Shader/WhittedShader.hpp"
#include "Shader/DistributedShader.hpp"
#include "Shader/PathTracerShader.hpp"
#include "Light/AmbientLight.hpp"
#include "Light/PointLight.hpp"

int main(int argc, const char * argv[]) {
    Scene scene;
    CfgReader cfgReader;
    cfgReader.getInfo();
    Camera *cam = nullptr;
    Image *img = nullptr;
    Shader *shd;
    bool success;

    std::string filepath = "../libs/tinyobjloader/models/cornell_box_VI.obj";

    std::string source_dir = __FILE__;
    source_dir = source_dir.substr(0, source_dir.find_last_of("/\\"));

    std::string fullpath = std::filesystem::path(source_dir).string() + "/" + filepath;
    std::string fullpath_save = std::filesystem::path(source_dir).string() + "/" + cfgReader.settings.outputname;

    success = scene.Load(fullpath);

    std::cout << fullpath << "\n";
    
    if (!success) {
        std::cout << "ERROR!! :o\n";
        return 1;
    }
    std::cout << "Scene Load: SUCCESS!! :-)\n";
    std::cout << std::endl;
    
    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.25,0.25,0.25));
    scene.lights.push_back(ambient);
    scene.numLights++;
    // add a point light to the scene
    // PointLight* pl1 = new PointLight(RGB(0.65, 0.65, 0.65), Point(288, 540, 282));
    // scene.lights.push_back(pl1);
    // scene.numLights++;
    // add a point light to the scene
    AreaLight *pl2 = new AreaLight(RGB(0.55, 0.55, 0.55), Point(238, 548, 232), Point(238, 548, 332), Point(338, 548, 232), Vector(0., -1., 0.));
    scene.lights.push_back(pl2);
    scene.numLights++;
    AreaLight *pl3 = new AreaLight(RGB(0.55, 0.55, 0.55), Point(238, 548, 332), Point(338, 548, 232), Point(338, 548, 332), Vector(0., -1., 0.));
    scene.lights.push_back(pl3);
    scene.numLights++;
    // AreaLight *pl4 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(88, 548, 82), Point(88, 548, 112), Point(118, 548, 82), Vector(0., -1., 0.));
    // scene.lights.push_back(pl4);
    // scene.numLights++;
    // AreaLight *pl5 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(88, 548, 112), Point(118, 548, 82), Point(118, 548, 112), Vector(0., -1., 0.));
    // scene.lights.push_back(pl5);
    // scene.numLights++;
    // AreaLight *pl6 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(458, 548, 452), Point(458, 548, 482), Point(488, 548, 452), Vector(0., -1., 0.));
    // scene.lights.push_back(pl6);
    // scene.numLights++;
    // AreaLight *pl7 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(458, 548, 482), Point(488, 548, 452), Point(488, 548, 482), Vector(0., -1., 0.));
    // scene.lights.push_back(pl7);
    // scene.numLights++;
    // AreaLight *pl8 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(458, 548, 82), Point(458, 548, 112), Point(488, 548, 82), Vector(0., -1., 0.));
    // scene.lights.push_back(pl8);
    // scene.numLights++;
    // AreaLight *pl9 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(458, 548, 112), Point(488, 548, 82), Point(488, 548, 112), Vector(0., -1., 0.));
    // scene.lights.push_back(pl9);
    // scene.numLights++;
    // AreaLight *pl10 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(88, 548, 452), Point(88, 548, 482), Point(118, 548, 452), Vector(0., -1., 0.));
    // scene.lights.push_back(pl10);
    // scene.numLights++;
    // AreaLight *pl11 = new AreaLight(RGB(0.35, 0.35, 0.35), Point(88, 548, 482), Point(118, 548, 452), Point(118, 548, 482), Vector(0., -1., 0.));
    // scene.lights.push_back(pl11);
    // scene.numLights++;
    
    scene.printSummary();
    // scene.printInfo();
    
    // Image resolution
    const int W= 1024;
    const int H= 1024;
    
    if (cfgReader.settings.outputtype == "PPM") {

        img = new ImagePPM(W,H);

    } else if (cfgReader.settings.outputtype == "JPG") {

        img = new ImageJPG(W,H);

    } else if (cfgReader.settings.outputtype == "PFM") {

        img = new ImagePFM(W,H);

    } else if (cfgReader.settings.outputtype == "EXR") {

        img = new ImageEXR(W,H);

    } else return -1;
    
    if (cfgReader.settings.cameratype == "perspective") {

        const Point Eye ={280,275,-330}, At={280,265,0};
        const Vector Up={0,1,0};
        const float fovW =  90;
        const float fovH = fovW * ((float) H/ (float)W);
        cam = new Perspective(Eye, At, Up, W, H, fovW, fovH);

    } else if (cfgReader.settings.cameratype == "environment") {

        const Point EyeEnv ={280,275,-50}, AtEnv={280,265,0};
        const Vector Up={0,1,0};
        cam = new EnvironmentCamera(W, H, EyeEnv, AtEnv, Up);

    } else return -1;

    // create the shader
    RGB background(0.05, 0.05, 0.55);
    // shd = new AmbientShader(&scene, background);

    // shd = new WhittedShader(&scene, background);

    shd = new DistributedShader(&scene, background);

    // shd = new PathTracerShader(&scene, background);

    // declare the renderer
    StandardRenderer myRender (cam, &scene, img, shd);

    myRender.Render();

    img->Save(fullpath_save);
    
    std::cout << "That's all, folks!" << std::endl;
    return 0;
}
