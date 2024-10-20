//
//  Ray.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef Ray_hpp
#define Ray_hpp

#include "../utils/vector.hpp"
#include <limits>

const float Infinity = std::numeric_limits<float>::infinity();
const float EPSILON=1e-3;

class Ray {
public:
    Point o; // ray origin
    Vector dir; // ray direction
    float tMin;     // Minimum ray parameter value
    float tMax;     // Maximum ray parameter value
    int x,y;  // pixel associated with this pixel
    Ray () {}
    Ray (Point o, Vector d, float tMin = 0.0f, float tMax = Infinity, float time = 0.0f): o(o),dir(d),tMin(tMin), tMax(tMax) {}
    ~Ray() {}
    void adjustOrigin (Vector normal) {
        Vector offset = EPSILON * normal;
        if (dir.dot(normal) < 0)
            offset = -1.f * offset;
        o.X = o.X + offset.X;
        o.Y = o.Y + offset.Y;
        o.Z = o.Z + offset.Z;
    }
};

#endif /* Ray_hpp */