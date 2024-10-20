//
//  ImageEXR.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImageEXR.hpp"
#include <iostream>
#include <fstream>
#include <vector>

void ImageEXR::ToneMap() {

    imageToSave = new EXR_pixel[this->W*this->H];

    for (int j = 0 ; j< H ; j++) {

        for (int i = 0; i < W ; ++i) {

            imageToSave[j*W+i].val[0] = std::min(1.f, imagePlane[j*W+i].R);
            imageToSave[j*W+i].val[1] = std::min(1.f, imagePlane[j*W+i].G);
            imageToSave[j*W+i].val[2] = std::min(1.f, imagePlane[j*W+i].B);
        }
    }
}

bool ImageEXR::Save(std::string filename) {

    ToneMap();

    std::vector<Imf::Rgba> pixels(W * H);
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; i++) {

            pixels[j * W + i] = Imf::Rgba(imageToSave[j*W+i].val[0], imageToSave[j*W+i].val[1], imageToSave[j*W+i].val[2]);
        }
    }

    Imf::RgbaOutputFile file(filename.c_str(), W, H, Imf::WRITE_RGBA);

    file.setFrameBuffer(&pixels[0], 1, W);
    file.writePixels(H);

    return true;
}
