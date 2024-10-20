//
//  Phong.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 07/02/2023.
//

// #include "Phong.hpp"


// RGB Phong::sampleTexture(float u, float v) {
//     // Ensure texture coordinates are within the range [0, 1]
//     u = std::clamp(u, 0.0f, 1.0f);
//     v = std::clamp(v, 0.0f, 1.0f);

//     // Convert texture coordinates to pixel coordinates
//     int x = static_cast<int>(u * (this->texWidth - 1));
//     int y = static_cast<int>(v * (this->texHeigth - 1));

//     // Get the color of the corresponding pixel in the texture image
//     int pixelIndex = y * this->texWidth + x;
//     RGB color = this->texture[pixelIndex];

//     return color;
// }
