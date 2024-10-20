//
//  mesh.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 05/02/2023.
//

#ifndef mesh_hpp
#define mesh_hpp

#include "geometry.hpp"
#include "../../utils/vector.hpp"
#include <vector>
#include <cfloat>

// partially inspired in pbrt book (3rd ed.), sec 3.6, pag 152

typedef struct Face
{
    int vert_ndx[3];         // indices to our internal vector of vertices (in Mesh)
    int textVert_ndx[3];
    Vector geoNormal;        // geometric normal
    bool hasShadingNormals;  // are there per vertex shading normals ??
    int vert_normals_ndx[3]; // indices to veritices normals
    BB bb;                   // face bounding box
                             // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
} Face;

class Mesh : public Geometry {

private:

    void calcBary(Face f, Intersection* isect) {
        // Calculate the barycentric coordinates of the intersection point within the triangle
        Vector v0 = this->vertices[f.vert_ndx[0]].vec2point(this->vertices[f.vert_ndx[1]]);
        Vector v1 = this->vertices[f.vert_ndx[0]].vec2point(this->vertices[f.vert_ndx[2]]);
        Vector v2 = this->vertices[f.vert_ndx[0]].vec2point(isect->p);

        float d00 = v0.dot(v0);
        float d01 = v0.dot(v1);
        float d11 = v1.dot(v1);
        float d20 = v2.dot(v0);
        float d21 = v2.dot(v1);

        float denom = d00 * d11 - d01 * d01;
        float barycentricV = (d11 * d20 - d01 * d21) / denom;
        float barycentricW = (d00 * d21 - d01 * d20) / denom;
        float barycentricU = 1.0f - barycentricV - barycentricW;

        Point2D texCoord1 = this->texVertices[f.textVert_ndx[0]];
        Point2D texCoord2 = this->texVertices[f.textVert_ndx[1]];
        Point2D texCoord3 = this->texVertices[f.textVert_ndx[2]];

        // Interpolate the texture coordinates using the barycentric coordinates
        float interpolatedU = barycentricU * texCoord1.X + barycentricV * texCoord2.X + barycentricW * texCoord3.X;
        float interpolatedV = barycentricU * texCoord1.Y + barycentricV * texCoord2.Y + barycentricW * texCoord3.Y;

        isect->uv[0] = interpolatedU;
        isect->uv[1] = interpolatedV;
    }


    bool TriangleIntersect(Ray r, Face f, Intersection* isect) {
        // intersect the ray with the mesh BB
        if (!bb.intersect(r))
            return false;

        // Retrieve triangle vertices
        Point v0 = vertices[f.vert_ndx[0]];
        Point v1 = vertices[f.vert_ndx[1]];
        Point v2 = vertices[f.vert_ndx[2]];

        // Calculate the triangle's normal
        Vector e1 = v0.vec2point(v1);
        Vector e2 = v0.vec2point(v2);
        Vector n = e1.cross(e2);

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
        Vector normal = f.geoNormal;
        isect->wo = -1.f * r.dir;
        // make sure the normal points to the same side of the surface as wo
        normal = normal.Faceforward(isect->wo);
        isect->gn = normal;
        isect->sn = normal;

        // Update the intersection object
        isect->p = Point(p.X, p.Y, p.Z);
        isect->depth = dist;
        this->calcBary(f, isect);

        return true;
    }

public:
    int numFaces;
    std::vector<Face> faces;
    int numVertices;
    std::vector<Point> vertices;
    std::vector<Point2D> texVertices;
    int numNormals;
    std::vector<Vector> normals;
    bool intersect(Ray r, Intersection *isect)
    {
        bool intersect = true, intersect_this_face;
        Intersection min_isect, curr_isect;
        float min_depth = MAXFLOAT;
        // intersect the ray with the mesh BB
        if (!bb.intersect(r))
            return false;

        // If it intersects then loop through the faces
        intersect = false;
        for (auto face_it = faces.begin(); face_it != faces.end(); face_it++)
        {
            intersect_this_face = TriangleIntersect(r, *face_it, &curr_isect);
            if (!intersect_this_face)
                continue;
            intersect = true;
            if (curr_isect.depth < min_depth)
            { // this is closer
                min_depth = curr_isect.depth;
                min_isect = curr_isect;
            }
        }

        // isect->depth = min_depth;
        *isect = min_isect;
        
        return intersect;
    }

    Mesh() : numFaces(0), numVertices(0), numNormals(0), Geometry() {}
};

#endif /* mesh_hpp */
