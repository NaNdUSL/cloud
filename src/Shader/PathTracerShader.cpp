//
//  PathTracerShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "PathTracerShader.hpp"
#include "../Primitive/BRDF/Phong.hpp"
#include "../Light/AreaLight.hpp"

RGB PathTracerShader::shade(bool intersected, Intersection isect, int depth) {
     RGB color(0., 0., 0.);

    if (!intersected) return (background);

    // intersection with a light source
    if (isect.isLight) return isect.Le;

    // get the BRDF
    Phong* f = (Phong*)isect.f;

    float rnd_russian = ((float)rand()) / ((float)RAND_MAX);
    if (depth < MAX_DEPTH || rnd_russian < continue_p) {
        RGB lcolor;

        // random select between specular and diffuse
        float s_p = f->Ks.Y() / (f->Ks.Y() + f->Kd.Y());
        float rnd = ((float)rand()) / ((float)RAND_MAX);

        if (rnd < s_p) // do specular
            lcolor = specularReflection(isect, f, depth + 1) * (1.0f / s_p);
        else // do diffuse
            lcolor = diffuseReflection(isect, f, depth + 1) * (1.0f / (1. - s_p));
        color += lcolor;
        if (depth < MAX_DEPTH) // No Russian roulette
            color += lcolor;
        else color += lcolor * (1.0f / continue_p);
    }
    // if there is a diffuse component do direct light
    if (!f->Kd.isZero()) color += directLighting(isect, f);

    return color;
};

RGB PathTracerShader::directLighting(Intersection isect, Phong* f) {
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
            if (!f->Kd.isZero() || f->texHeigth) {
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
                        if (scene->visibility(shadow, Ldistance - EPSILON)) { // light source not occluded
                            if(f->texHeigth) {
                                color = f->sampleTexture(isect.uv[0], isect.uv[1]) * L * cosL;
                            } else {
                                color += f->Kd * L * cosL;
                            }
                        } 
                            
                    }
                } // end cosL > 0.
            }
            continue;
        }
        if ((*l)->type == AREA_LIGHT) { // is it an area light ?
            if (!f->Kd.isZero() || f->texHeigth) {
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

int temp = 0;

RGB PathTracerShader::specularReflection(Intersection isect, Phong* f, int depth) {
    RGB color(0.,0.,0.); Vector Rdir, s_dir; float pdf; Intersection s_isect;

    float cos = isect.gn.dot(isect.wo);
    Rdir = 2.f * cos * isect.gn - isect.wo;


    if (f->Ns >= 1000) { // ideal specular
        Ray specular(isect.p, Rdir);
        specular.adjustOrigin(isect.gn);
        // trace ray
        bool intersected = scene->trace(specular, &s_isect);
        RGB Rcolor = shade (intersected, s_isect, depth+1);

        if(f->texHeigth) {
            color = f->sampleTexture(isect.uv[0], isect.uv[1]) * Rcolor;
        } else {
            color = (f->Ks * Rcolor);
        }
        return color;
    }

    else if (f->Ns < 1000) { // glossy materials
        float rnd[2] = { (float)rand(), (float)rand() };

        Vector S_around_N;

        const float cos_theta = powf(rnd[1], 1./(f->Ns+1.));
        S_around_N.Z = cos_theta;

        const float aux_r1 = powf(rnd[1], 2./(f->Ns+1.));
        S_around_N.Y = sinf(2.*M_PI*rnd[0])*sqrtf(1.-aux_r1);
        S_around_N.X = cosf(2.*M_PI*rnd[0])*sqrtf(1.-aux_r1);
        pdf = (f->Ns+1.f)*powf(cos_theta, f->Ns)/(2.f*M_PI);
        
        Vector Rx, Ry;

        Rdir.CoordinateSystem(&Rx, &Ry);
        s_dir = S_around_N.Rotate (Rx, Ry, Rdir);
        
        Ray specular(isect.p, s_dir);
        specular.adjustOrigin(isect.gn);
        
        bool intersected = scene->trace(specular, &s_isect);
        RGB Rcolor = shade (intersected, s_isect, depth+1);
        
        RGB Ks = f->Ks;
        if(f->texHeigth) {
            Ks = f->sampleTexture(isect.uv[0], isect.uv[1]);
        }

        color = (Ks * Rcolor * powf(cos_theta, f->Ns) * (1.f / (2.f*M_PI))) * (1.f / pdf);

        // std::cout << color.R << ", " << color.G << ", " << color.B << "\n";
    
        return color;
    }

    return color;
}
int diftemp = 0;
RGB PathTracerShader::diffuseReflection(Intersection isect, Phong* f, int depth) {
    RGB color(0., 0., 0.); Vector dir; float pdf;

    // actual direction distributed around N: 2 random number in [0,1[
    float rnd[2] = { (float)rand(), (float)rand() };

    Vector D_around_Z;
    float cos_theta = D_around_Z.Z = sqrtf(rnd[1]); // cos sampling
    D_around_Z.Y = sinf(2. * M_PI * rnd[0]) * sqrtf(1. - rnd[1]);
    D_around_Z.X = cosf(2. * M_PI * rnd[0]) * sqrtf(1. - rnd[1]);
    pdf = cos_theta / (M_PI);

    // generate a coordinate system from N
    Vector Rx, Ry;
    isect.gn.CoordinateSystem(&Rx, &Ry);

    Ray diffuse(isect.p, D_around_Z.Rotate(Rx, Ry, isect.gn));
    // OK, we have the ray : trace and shade it recursively
    Intersection d_isect;
    bool intersected = scene->trace(diffuse, &d_isect);

    // if light source return 0 ; handled by direct
    if (!d_isect.isLight) { // shade this intersection
        RGB Rcolor = shade(intersected, d_isect, depth + 1);

        RGB Kd = f->Kd;
        if(f->texHeigth) {
            Kd = f->sampleTexture(isect.uv[0], isect.uv[1]);
        }
        color = (Kd * cos_theta * Rcolor) * (1.f / pdf);
    }

    return color;
}
