//
//  mesh.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#include "mesh.hpp"

// see pbrt book (3rd ed.), sec 3.6.2, pag 157
// bool Mesh::TriangleIntersect (Ray r, Face f, Intersection *isect) {

//     if (!f.bb.intersect(r)) return false;

//     // Get the three vertices of the triangle.
//     Point v0 = vertices[f.vert_ndx[0]];
//     Point v1 = vertices[f.vert_ndx[1]];
//     Point v2 = vertices[f.vert_ndx[2]];

//     // Compute triangle normal
//     Vector e1 = v0.vec2point(v1);
//     Vector e2 = v0.vec2point(v2);
//     Vector normal = e1.cross(e2);

//     // Compute distance to plane
//     Vector dif = Vector(v0.X - r.o.X, v0.Y - r.o.Y, v0.Z - r.o.Z);
//     float d = normal.dot(dif) / normal.dot(r.dir);
//     if (d < 0.0f) {
//         // Triangle is behind the ray origin
//         return false;
//     }

//     // Compute intersection point
//     Point p = r.o + Point(d * r.dir.X, d * r.dir.Y, d * r.dir.Z);

//     // Check if intersection point is inside the triangle
//     Vector e0 = p.vec2point(v0);
//     float u = normal.dot(e0.cross(e1));
//     float v = normal.dot(e2.cross(e0));
//     if (u < 0.0f || v < 0.0f || u + v > 1.0f) {
//         // Intersection point is outside the triangle
//         return false;
//     }
    
//     // Compute intersection information and store in isect
//     isect->p = p;
//     isect->gn = normal;
//     isect->sn = normal; // Shading normal is the same as the geometric normal for now
//     isect->wo = r.dir * -1.0f;
//     isect->depth = d;
//     // Set BRDF to NULL for now, this will be set later by the material
//     isect->f = NULL;
    
//     return true;
// }


// bool Mesh::intersect (Ray r, Intersection *isect) {
//     bool intersect = true, intersect_this_face;
//     Intersection min_isect, curr_isect;
//     float min_depth=MAXFLOAT;

//     // intersect the ray with the mesh BB
//     if (!bb.intersect(r)) return false;
    
//     // If it intersects then loop through the faces
//     intersect = false;
//     for (auto face_it=faces.begin() ; face_it != faces.end() ; face_it++) {
//         intersect_this_face = TriangleIntersect(r, *face_it, &curr_isect);
//         if (!intersect_this_face) continue;
        
//         intersect = true;
//         if (curr_isect.depth < min_depth) {  // this is closer
//             min_depth = curr_isect.depth;
//             min_isect = curr_isect;
//         }
//     }
    
//     return intersect;
// }
