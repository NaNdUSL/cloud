#ifndef environmentcamera_hpp
#define environmentcamera_hpp

#include "camera.hpp"
#include "../Rays/ray.hpp"
#include "../utils/vector.hpp"

class EnvironmentCamera: public Camera {
private:
    int W;
    int H;
    Point Eye, At;
    Vector Up;
    float c2w[3][3];  // camera 2 world transform

public:
    EnvironmentCamera(int w, int h, Point Eye, Point At, const Vector Up)
        : W(w), H(h), Eye(Eye), At(At), Up(Up) {
            // Compute camera coordinate frame
            Vector f = Eye.vec2point(At);
            f.normalize();
            Vector r = f.cross(Up);
            r.normalize();
            Vector u = Up;

            // Create camera-to-world transform matrix
            c2w[0][0] = r.X;
            c2w[0][1] = r.Y;
            c2w[0][2] = r.Z;
            c2w[1][0] = u.X;
            c2w[1][1] = u.Y;
            c2w[1][2] = u.Z;
            c2w[2][0] = f.X;
            c2w[2][1] = f.Y;
            c2w[2][2] = f.Z;
    }
    bool GenerateRay(const int x, const int y, Ray* r, const float* cam_jitter = nullptr);
    void getResolution (int *_W, int *_H) {*_W=W; *_H=H;};
};

#endif /* environmentcamera_hpp */
