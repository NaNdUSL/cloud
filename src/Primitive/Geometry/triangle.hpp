//
//  mesh.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include "geometry.hpp"
#include "../../utils/vector.hpp"
#include <vector>
#include <cfloat>

class Triangle : public Geometry {
public:
	Point v1, v2, v3;
	Vector normal, edge1, edge2, edge3; 
	BB bb;

	bool TriangleIntersect(Ray r, Intersection* isect) {
        // intersect the ray with the mesh BB
        if (!bb.intersect(r))
            return false;

        // Retrieve triangle vertices
        Point v0 = v1;
        Point v1 = v2;
        Point v2 = v3;

        // Calculate the triangle's normal
        Vector e1 = edge1;
        Vector e2 = edge2;
        Vector n = normal;

        // Check if the ray is parallel to the triangle
        float denom = r.dir.dot(n);
        if (std::fabs(denom) < 1e-6f) {
            return false;
        }

        // Compute the intersection point between the ray and the triangle's plane
        float t = n.dot(r.o.vec2point(v0)) / denom;
        if (t < 0) {
            return false;
        }

        // Calculate the intersection point
        Vector p = Point(0,0,0).vec2point(r.o) + r.dir * t;

        // Check if the intersection point is inside the triangle
        Vector w = p - Point(0,0,0).vec2point(v0);
        float uu = e1.dot(e1);
        float uv = e1.dot(e2);
        float vv = e2.dot(e2);
        float wu = w.dot(e1);
        float wv = w.dot(e2);
        float D = uv * uv - uu * vv;

        // Calculate barycentric coordinates
        float s = (uv * wv - vv * wu) / D;
        if (s < 0 || s > 1) {
            // Intersection point is outside the triangle along the u-axis
            return false;
        }

        t = (uv * wu - uu * wv) / D;
        if (t < 0 || t > 1 - s) {
            // Intersection point is outside the triangle along the v-axis or outside the triangle bounds
            return false;
        }

        // Calculate the distance from the ray origin to the intersection point
        float dist = std::sqrt((r.o.X - p.X) * (r.o.X - p.X) + (r.o.Y - p.Y) * (r.o.Y - p.Y) + (r.o.Z - p.Z) * (r.o.Z - p.Z));

        // Fill Intersection data
        Vector normal = this->normal;
        isect->wo = -1.f * r.dir;
        // make sure the normal points to the same side of the surface as wo
        normal = normal.Faceforward(isect->wo);
        isect->gn = normal;
        isect->sn = normal;

        // Update the intersection object
        isect->p = Point(p.X, p.Y, p.Z);
        isect->depth = dist;
        return true;
    }
	
	bool intersect(Ray r, Intersection *isect)
    {
        bool intersect = true;
        Intersection min_isect, curr_isect;
        float min_depth = MAXFLOAT;
        // intersect the ray with the mesh BB
        if (!bb.intersect(r))
            return false;
		
        // If it intersects then loop through the faces
		intersect = TriangleIntersect(r, &curr_isect);

		if (curr_isect.depth < min_depth)
		{ // this is closer
			min_depth = curr_isect.depth;
			min_isect = curr_isect;
		}

        // isect->depth = min_depth;
        *isect = min_isect;
        
        return intersect;
    }
	Triangle(Point _v1, Point _v2, Point _v3, Vector _normal) :
		v1(_v1), v2(_v2), v3(_v3), normal(_normal) {
		edge1 = v1.vec2point(v2); edge2 = v1.vec2point(v3); edge3 = v2.vec2point(v3);
		bb.min.set(v1.X, v1.Y, v1.Z); bb.max.set(v1.X, v1.Y, v1.Z);
		bb.update(v2); bb.update(v3);
	}
	// https://www.mathopenref.com/heronsformula.html
	float area() {

		float a = edge1.norm();
		float b = edge2.norm();
		float c = edge3.norm();

		float p = (a + b + c) * 0.5f;
		float area = sqrt(p * (p - a) * (p - b) * (p - c));

		return area;
	}
};

#endif /* triangle_hpp */
