#include "environment.hpp"
#include "../Image/image.hpp"
#include "../Rays/intersection.hpp"
#include "environment.hpp"

bool EnvironmentCamera::GenerateRay(const int x, const int y, Ray* ray, const float *cam_jitter) {
    // Compute the spherical coordinates from the pixel positions
    float theta = static_cast<float>(M_PI) * static_cast<float>(y) / static_cast<float>(H);
    float phi = 2.0f * static_cast<float>(M_PI) * static_cast<float>(x) / static_cast<float>(W);

    // Apply camera jitter if provided
    float jitterX = cam_jitter[0];
    float jitterY = cam_jitter[1];
    float jitterTheta = jitterY * static_cast<float>(M_PI) / static_cast<float>(H);
    float jitterPhi = jitterX * 2.0f * static_cast<float>(M_PI) / static_cast<float>(W);
    theta += jitterTheta;
    phi += jitterPhi;

    // Compute the direction in camera space using spherical coordinates
    Vector dir(-(std::sin(theta) * std::cos(phi)), std::cos(theta), std::sin(theta) * std::sin(phi));

    // Transform the direction from camera space to world space
    Vector dirWorld(dir.X * c2w[0][0] + dir.Y * c2w[1][0] + dir.Z * c2w[2][0],
                    dir.X * c2w[0][1] + dir.Y * c2w[1][1] + dir.Z * c2w[2][1],
                    dir.X * c2w[0][2] + dir.Y * c2w[1][2] + dir.Z * c2w[2][2]);

    // Set the origin, direction, and other properties of the Ray structure
    ray->o = Eye;
    ray->dir = dirWorld;
    ray->tMin = 0.0f;
    ray->tMax = Infinity;

    return true;
}