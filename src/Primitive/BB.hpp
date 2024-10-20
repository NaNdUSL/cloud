//
//  AABB.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef BB_hpp
#define BB_hpp

#include "../utils/vector.hpp"
#include "../Rays/ray.hpp"
#include <algorithm>
#include <iostream>

typedef struct BB {
    Point min, max;
    void update (Point p) {
        if (p.X < min.X) min.X = p.X;
        else if (p.X > max.X) max.X = p.X;
        if (p.Y < min.Y) min.Y = p.Y;
        else if (p.Y > max.Y) max.Y = p.Y;
        if (p.Z < min.Z) min.Z = p.Z;
        else if (p.Z > max.Z) max.Z = p.Z;
    }
    bool intersect(Ray r) const {

        float tmin = (min.X - r.o.X) / r.dir.X;
        float tmax = (max.X - r.o.X) / r.dir.X;
        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (min.Y - r.o.Y) / r.dir.Y;
        float tymax = (max.Y - r.o.Y) / r.dir.Y;
        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax)) return false;

        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        float tzmin = (min.Z - r.o.Z) / r.dir.Z;
        float tzmax = (max.Z - r.o.Z) / r.dir.Z;
        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax)) return false;

        return true;
    }

    // Check if a ray intersects a bounding box
// bool intersect(Ray ray) {
//     double tmin = -INFINITY;
//     double tmax = INFINITY;

//     // Check for intersection in the x-axis
//     if (ray.dir.X != 0) {
//         double tx1 = (min.X - ray.o.X) / ray.dir.X;
//         double tx2 = (max.X - ray.o.X) / ray.dir.X;
//         tmin = std::max(tmin, std::min(tx1, tx2));
//         tmax = std::min(tmax, std::max(tx1, tx2));
//     } else if (ray.o.X < min.X || ray.o.X > max.X) {
//         return false;
//     }

//     // Check for intersection in the y-axis
//     if (ray.dir.Y != 0) {
//         double ty1 = (min.Y - ray.o.Y) / ray.dir.Y;
//         double ty2 = (max.Y - ray.o.Y) / ray.dir.Y;
//         tmin = std::max(tmin, std::min(ty1, ty2));
//         tmax = std::min(tmax, std::max(ty1, ty2));
//     } else if (ray.o.Y < min.Y || ray.o.Y > max.Y) {
//         return false;
//     }

//     // Check for intersection in the z-axis
//     if (ray.dir.Z != 0) {
//         double tz1 = (min.Z - ray.o.Z) / ray.dir.Z;
//         double tz2 = (max.Z - ray.o.Z) / ray.dir.Z;
//         tmin = std::max(tmin, std::min(tz1, tz2));
//         tmax = std::min(tmax, std::max(tz1, tz2));
//     } else if (ray.o.Z < min.Z || ray.o.Z > max.Z) {
//         return false;
//     }

//     // If tmin is greater than or equal to tmax, the ray does not intersect the box
//     if (tmin >= tmax) {
//         return false;
//     }

//     // Otherwise, the ray intersects the box
//     return true;
// }
} BB;

#endif /* AABB_hpp */
