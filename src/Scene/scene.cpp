//
//  Scene.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include "scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../libs/tinyobjloader/tiny_obj_loader.h"
#include "../Primitive/primitive.hpp"
#include "../Primitive/Geometry/mesh.hpp"
#include "../utils/RGB.hpp"

#include <iostream>
#include <set>
#include <vector>

using namespace tinyobj;

static void PrintInfo(const ObjReader myObj)
{
    const tinyobj::attrib_t attrib = myObj.GetAttrib();
    const std::vector<tinyobj::shape_t> shapes = myObj.GetShapes();
    const std::vector<tinyobj::material_t> materials = myObj.GetMaterials();
    std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
    std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
              << std::endl;

    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;

    // Iterate shapes
    auto it_shape = shapes.begin();
    for (; it_shape != shapes.end(); it_shape++)
    {
        // assume each face has 3 vertices
        // std::cout << "Processing shape " << it_shape->name << std::endl;
        // iterate faces
        // assume each face has 3 vertices
        auto it_vertex = it_shape->mesh.indices.begin();
        for (; it_vertex != it_shape->mesh.indices.end();)
        {
            // process 3 vertices
            for (int v = 0; v < 3; v++)
            {
                // std::cout << it_vertex->vertex_index;
                it_vertex++;
            }
            // std::cout << std::endl;
        }
        // std::cout << std::endl;

        printf("There are %lu material indexes\n", it_shape->mesh.material_ids.size());
    }
}

/*
 Use tiny load to load .obj scene descriptions
 https://github.com/tinyobjloader/tinyobjloader
 */

bool Scene::Load(const std::string &fname)
{

    ObjReader myObjReader;

    if (!myObjReader.ParseFromFile(fname))
    {
        std::cerr << "Error loading OBJ file: " << myObjReader.Error() << std::endl;
        return false;
    }

    // PrintInfo (myObjReader);

    // convert loader's representation to my representation
    auto &attrib = myObjReader.GetAttrib();
    auto &shapes = myObjReader.GetShapes();
    auto &materials = myObjReader.GetMaterials();
    int minIndex = 0;
    int maxIndex = 0;

    // Loop over shapes
    for (tinyobj::shape_t shape : shapes)
    {
        // Create a new mesh for each shape
        Mesh *mesh = new Mesh;

        for (size_t i = 0; i < shape.mesh.indices.size(); i += 3)
        {
            const auto &idx1 = shape.mesh.indices[i];
            const auto &idx2 = shape.mesh.indices[i + 1];
            const auto &idx3 = shape.mesh.indices[i + 2];

            Face *face = new Face;
            face->vert_ndx[0] = idx1.vertex_index - minIndex;
            face->vert_ndx[1] = idx2.vertex_index - minIndex;
            face->vert_ndx[2] = idx3.vertex_index - minIndex;

            face->textVert_ndx[0] = idx1.texcoord_index - minIndex;
            face->textVert_ndx[1] = idx2.texcoord_index - minIndex;
            face->textVert_ndx[2] = idx3.texcoord_index - minIndex;

            Point *pf1 = new Point(attrib.vertices[3 * idx1.vertex_index + 0], attrib.vertices[3 * idx1.vertex_index + 1], attrib.vertices[3 * idx1.vertex_index + 2]);
            Point *pf2 = new Point(attrib.vertices[3 * idx2.vertex_index + 0], attrib.vertices[3 * idx2.vertex_index + 1], attrib.vertices[3 * idx2.vertex_index + 2]);
            Point *pf3 = new Point(attrib.vertices[3 * idx3.vertex_index + 0], attrib.vertices[3 * idx3.vertex_index + 1], attrib.vertices[3 * idx3.vertex_index + 2]);

            // Calculate the normal vector of the face
            Vector n = pf2->vec2point(*pf1).cross(pf3->vec2point(*pf1));
            n.normalize();
            face->geoNormal = n;

            face->bb.update(*pf1);
            face->bb.update(*pf2);
            face->bb.update(*pf3);

            mesh->faces.push_back(*face);

            if (idx1.vertex_index > maxIndex)
                maxIndex = idx1.vertex_index;
            if (idx2.vertex_index > maxIndex)
                maxIndex = idx2.vertex_index;
            if (idx3.vertex_index > maxIndex)
                maxIndex = idx3.vertex_index;

            // access to normal
            // if (idx.normal_index >= 0) {
            //     Vector normal(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]);
            //     mesh->normals.push_back(normal);
            // }

            // // access to texcoord
            // if (idx.texcoord_index >= 0) {
            //     tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
            //     tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
            // }
        }

        for (size_t i = minIndex; i < maxIndex + 1; i++)
        {
            // access to vertex
            Point *point = new Point(attrib.vertices[3 * i + 0], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);
            mesh->bb.update(*point);

            mesh->vertices.push_back(*point);
            
            if(!attrib.texcoords.empty()) {
                Point2D *texPoint = new Point2D(attrib.texcoords[2 * i + 0], attrib.texcoords[2 * i + 1]);
                mesh->texVertices.push_back(*texPoint);
            }
        }

        minIndex = maxIndex + 1;

        // Create a new primitive for each mesh and material
        Primitive *primitive = new Primitive;
        primitive->material_ndx = shape.mesh.material_ids[0];
        primitive->g = mesh;
        this->prims.push_back(primitive);
        this->numPrimitives++;
    }

    this->addMaterials(materials);

    return true;
}

bool Scene::trace(Ray r, Intersection *isect)
{
    Intersection curr_isect;
    bool intersection = false;

    if (numPrimitives == 0)
        return false;

    // iterate over all primitives

    for (auto prim_itr = prims.begin(); prim_itr != prims.end(); prim_itr++)
    {
        if ((*prim_itr)->g->intersect(r, &curr_isect))
        {

            if (!intersection)
            { // first intersection
                intersection = true;
                // std::cout << "dist: " << isect->depth << std::endl;
                *isect = curr_isect; 
                // isect->depth = curr_isect.depth;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
            else if (curr_isect.depth < isect->depth)
            {
                // std::cout << "Better intersection" << std::endl;
                *isect = curr_isect;
                // isect->depth = curr_isect.depth;
                isect->f = BRDFs[(*prim_itr)->material_ndx];
            }
        }
    }

    isect->isLight = false; // download new intersection.hpp
    // now iterate over light sources and intersect with those that have geometry
    for (auto l = lights.begin(); l != lights.end(); l++) {
        if ((*l)->type == AREA_LIGHT) {
            AreaLight* al = (AreaLight*)(*l);
            if (al->gem->intersect(r, &curr_isect)) {
                if (!intersection) { // first intersection
                    intersection = true;
                    *isect = curr_isect;
                    isect->isLight = true;
                    isect->Le = al->L();
                }
                else if (curr_isect.depth < isect->depth) {
                    *isect = curr_isect;
                    isect->isLight = true;
                    isect->Le = al->L();
                }
            }
        }
    }
    
    return intersection;
}

void Scene::addMaterials(std::vector<tinyobj::material_t> materials)
{
    for (tinyobj::material_t material : materials)
    {


        // ambient
        RGB *ambient = new RGB((float *)material.ambient);

        // diffuse
        RGB *diffuse = new RGB((float *)material.diffuse);

        // specular
        RGB *specular = new RGB((float *)material.specular);

        // specular
        int Ns = material.shininess;

        Phong *phong = new Phong;
        phong->Ka = *ambient;
        phong->Kd = *diffuse;
        phong->Ks = *specular;
        phong->Ns = Ns;
        phong->texWidth = 0;
        phong->texHeigth = 0;

        if(!material.diffuse_texname.empty()){
            this->readTextureJpeg(*phong, material.diffuse_texname);
        }
        this->addBRDF(phong);
    }
}

void Scene::readTextureJpeg(Phong &phong, std::string filename){
    std::string filepath = "../../libs/tinyobjloader/models/" + filename;
    std::string source_dir = __FILE__;
    source_dir = source_dir.substr(0, source_dir.find_last_of("/\\"));

    std::string fullpath = std::filesystem::path(source_dir).string() + "/" + filepath;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];
    
    FILE* infile = fopen(fullpath.c_str(), "rb");

    if (!infile) {
        std::cerr << "Failed to open file: " << fullpath << std::endl;
    }
    else {

        unsigned long location = 0;
        int i = 0;

        cinfo.err = jpeg_std_error( &jerr );

        jpeg_create_decompress( &cinfo );

        jpeg_stdio_src( &cinfo, infile );

        jpeg_read_header( &cinfo, TRUE );


        jpeg_start_decompress( &cinfo );

        unsigned char *raw_image = (unsigned char *)malloc(cinfo.image_width * cinfo.image_height * cinfo.num_components );

        row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );

        while( cinfo.output_scanline < cinfo.image_height )
        {
            jpeg_read_scanlines( &cinfo, row_pointer, 1 );
            for( i=0; i<cinfo.image_width*cinfo.num_components;i++) 
                raw_image[location++] = row_pointer[0][i];
        }

        for (int j = 0; j < cinfo.output_height; j++) {

            for (int i = 0; i < cinfo.output_width; i++) {

                RGB *pixel = new RGB;
                pixel->R = (float)(raw_image[(j * cinfo.output_width + i) * cinfo.num_components]) / 255.f;
                pixel->G = (float)(raw_image[(j * cinfo.output_width + i) * cinfo.num_components + 1]) / 255.f;
                pixel->B = (float)(raw_image[(j * cinfo.output_width + i) * cinfo.num_components + 2]) / 255.f;
                phong.texture.push_back(*pixel);
            }
        }

        phong.texWidth = cinfo.output_width;
        phong.texHeigth = cinfo.output_height;

        jpeg_finish_decompress( &cinfo );
        jpeg_destroy_decompress( &cinfo );
        free( row_pointer[0] );
        fclose( infile );
    }
}

void Scene::addBRDF(BRDF *BRDF)
{
    this->numBRDFs++;
    this->BRDFs.push_back(BRDF);
}

void Scene::increaseLights()
{
    this->numLights++;
}

void Scene::printInfo()
{
    // primitivas
    std::cout << "Primitivas, tamanho: " << this->prims.size() << "\n";
    for (const auto &primitive : this->prims)
    {
        std::cout << "  Material Id: " << primitive->material_ndx << "\n";

        Mesh *mesh = (Mesh *)primitive->g;

        // faces
        std::cout << "  Faces, tamanho: " << mesh->faces.size() << "\n";
        std::cout << "  Vertices, tamanho: " << mesh->vertices.size() << "\n";
        // for(Point point : mesh->vertices){
        //     std::cout << "point: (" << point.X << ", " << point.Y << ", " << point.Z << ")\n";
        // }
    }
    // materials
    std::cout << "Materiais, tamanho: " << this->BRDFs.size() << "\n";
    for (const auto &material : this->BRDFs)
    {
        Phong *p = (Phong *)material;

        // faces
        std::cout << "  Ka: (" << p->Ka.R << ", " << p->Ka.G << ", " << p->Ka.B << ")\n";
        std::cout << "  Kd: (" << p->Kd.R << ", " << p->Kd.G << ", " << p->Kd.B << ")\n";
        std::cout << "  Ks: (" << p->Ks.R << ", " << p->Ks.G << ", " << p->Ks.B << ")\n";
        std::cout << "  Ns: (" << p->Ns << ")\n";
    }
}

// checks whether a point on a light source (distance maxL) is visible
bool Scene::visibility (Ray s, const float maxL) {
    bool visible = true;
    Intersection curr_isect;
    
    if (numPrimitives==0) return true;
    
    // iterate over all primitives while visible
    for (auto prim_itr = prims.begin() ; prim_itr != prims.end() && visible ; prim_itr++) {
        if ((*prim_itr)->g->intersect(s, &curr_isect)) {
            if (curr_isect.depth < maxL) {
                visible = false;
            }
        }
    }

    return visible;
}
