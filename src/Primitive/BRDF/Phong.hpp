//
//  Phong.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 07/02/2023.
//

#ifndef Phong_hpp
#define Phong_hpp

#include "../../utils/RGB.hpp"
#include "BRDF.hpp"
#include <string>
#include <vector>
#include <algorithm>

class Phong: public BRDF {
public:
    RGB Ka, Kd, Ks, Kt;
    std::vector<RGB> texture = std::vector<RGB>();
    int texWidth;
    int texHeigth;
    int Ns;

    RGB sampleTexture(float u, float v) {
        // Ensure texture coordinates are within the range [0, 1]
        u = std::clamp(u, 0.0f, 1.0f);
        v = std::clamp(v, 0.0f, 1.0f);

        int x = static_cast<int>(u * (this->texWidth - 1));
        int y = static_cast<int>(v * (this->texHeigth - 1));

        // Get the color of the corresponding pixel in the texture image
        int pixelIndex = y * this->texWidth + x;
        RGB color = this->texture[pixelIndex];

        return color;
    }
};

#endif /* Phong_hpp */
