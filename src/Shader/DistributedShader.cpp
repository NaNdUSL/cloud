//
//  DistributedShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "DistributedShader.hpp"
#include "../Primitive/BRDF/Phong.hpp"
#include "../Light/AreaLight.hpp"

RGB DistributedShader::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0., 0., 0.);

    if (!intersected) return (background);

    // intersection with a light source
    if (isect.isLight) return isect.Le;

    Phong* f = (Phong*)isect.f;

    // if there is a specular component sample it
    if (!f->Ks.isZero()) color += specularReflection(isect, f);

    // if there is a diffuse component do direct light

    if (!f->Kd.isZero()) color += directLighting(isect, f);

    return color;
};

RGB DistributedShader::directLighting(Intersection isect, Phong* f) {
    RGB color(0., 0., 0.);

    for (auto l = scene->lights.begin(); l != scene->lights.end(); l++) {

        if ((*l)->type == AMBIENT_LIGHT) { // is it an ambient light ?

            if (!f->Ka.isZero()) {
                RGB Ka = f->Ka;
                if(f->texHeigth) {
                    Ka = f->sampleTexture(isect.uv[0], isect.uv[1]);
                }
                color += Ka * (*l)->L();
            }
            continue;
        }
        if ((*l)->type == POINT_LIGHT) { // is it a point light ?
            if (!f->Kd.isZero()) {
                Point lpoint;
                // get the position and radiance of the light source
                RGB L = (*l)->Sample_L(NULL, &lpoint);
                // compute the direction from the intersection to the light
                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                Ldir.normalize(); // now normalize Ldir
                // compute the cosine (Ldir , shading normal)
                float cosL = Ldir.dot(isect.sn);
                if (cosL > 0.) { // the light is NOT behind the primitive
                    // Add check to make sure the normal vector is pointing towards the Point Light
                    if (Ldir.dot(isect.sn) > 0) {
                        // generate the shadow ray
                        Ray shadow(isect.p, Ldir);
                        // adjust origin EPSILON along the normal: avoid self occlusion
                        shadow.adjustOrigin(isect.gn);
                        if (scene->visibility(shadow, Ldistance - EPSILON)){ // light source not occluded
                            RGB Kd = f->Kd;
                            if(f->texHeigth) {
                                Kd = f->sampleTexture(isect.uv[0], isect.uv[1]);
                            }
                            color += Kd * L * cosL;
                        }
                    }
                } // end cosL > 0.
            }
            continue;
        }
        if ((*l)->type == AREA_LIGHT) { // is it an area light ?
            if (!f->Kd.isZero()) {
                RGB L, Kd = f->Kd;
                if(f->texHeigth) {
                    Kd = f->sampleTexture(isect.uv[0], isect.uv[1]);
                }
                Point lpoint;
                float l_pdf;
                
                AreaLight* al = (AreaLight*)(*l);
                float rnd[2];
                rnd[0] = ((float)rand()) / ((float)RAND_MAX);
                rnd[1] = ((float)rand()) / ((float)RAND_MAX);
                L = al->Sample_L(rnd, &lpoint, l_pdf);
                
                // compute the direction from the intersection point to the light source
                Vector Ldir = isect.p.vec2point(lpoint);
                const float Ldistance = Ldir.norm();
                
                // now normalize Ldir
                Ldir.normalize();

                // cosine between Ldir and the shading normal at the intersection point
                float cosL = Ldir.dot(isect.sn);
                // cosine between Ldir and the area light source normal
                float cosL_LA = Ldir.dot(al->gem->normal);
                // shade
                if (cosL > 0. and cosL_LA <= 0.) { // light NOT behind primitive AND light normal points to the ray o
                        // generate the shadow ray
                        Ray shadow(isect.p, Ldir);
                        shadow.adjustOrigin(isect.gn);
                    if (scene->visibility(shadow, Ldistance - EPSILON)) { // light source not occluded
                        color += (Kd * L * cosL) * (1.0 / l_pdf);
                    }
                } // end cosL > 0.
            }
            continue;
        } // end area light
    }
    return color;
}

RGB DistributedShader::specularReflection(Intersection isect, Phong* f) {
    // generate the specular ray
    float cos = isect.gn.dot(isect.wo);
    Vector Rdir = 2.f * cos * isect.gn - isect.wo;
    Ray specular(isect.p, Rdir);
    specular.adjustOrigin(isect.gn);
    Intersection s_isect;
    // trace ray
    bool intersected = scene->trace(specular, &s_isect);
    // shade this intersection
    RGB color = shade(intersected, s_isect, 0);
    return color;
}
