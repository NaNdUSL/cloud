//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#include "perspective.hpp"
#include <iostream>

bool Perspective::GenerateRay(const int x, const int y, Ray* r, const float* cam_jitter) {
    float xc, yc;
    if (cam_jitter == NULL) {
        xc = 2.f * ((float)x + .5f) / W - 1.f;
        yc = 2.f * ((float)(H - y - 1) + .5f) / H - 1.f;
    }
    else {
        xc = 2.f * ((float)x + cam_jitter[0]) / W - 1.f;
        yc = 2.f * ((float)(H - y - 1) + cam_jitter[1]) / H - 1.f;
    }

    Vector dir_c = Vector(xc, yc, 1.0f);

    Vector* dir_w = new Vector;
    dir_w->X = c2w[0][0] * dir_c.X + c2w[0][1] * dir_c.Y + c2w[0][2] * dir_c.Z;
    dir_w->Y = c2w[1][0] * dir_c.X + c2w[1][1] * dir_c.Y + c2w[1][2] * dir_c.Z;
    dir_w->Z = c2w[2][0] * dir_c.X + c2w[2][1] * dir_c.Y + c2w[2][2] * dir_c.Z;

    // Set the origin and direction of the ray
    r->o = Eye;
    r->dir = *dir_w;
    return true;
}



 






