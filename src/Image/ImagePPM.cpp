//
//  ImagePPM.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImagePPM.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

void ImagePPM::ToneMap () {
    imageToSave = new PPM_pixel[this->W*this->H];

    for (int j = 0 ; j< H ; j++) {

        for (int i = 0; i < W ; ++i) {

            imageToSave[j*W+i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].R) * 255);
            imageToSave[j*W+i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].G) * 255);
            imageToSave[j*W+i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].B) * 255);
        }
    }

}

bool ImagePPM::Save (std::string filename) {
    
    ToneMap();

    std::ofstream myfile;
    myfile.open(filename, std::ios::binary); 

    if (!myfile){

        std::cout << "Error in creating file!!!\n";
        return 0;
    }

    myfile << "P6\n" << this->W << " " << this->H << "\n255\n"; 

    for (int j = 0; j < H; j++) {

        for (int i = 0; i < W; ++i) {

            myfile << this->imageToSave[j * W + i].val[0] << this->imageToSave[j * W + i].val[1] << this->imageToSave[j * W + i].val[2];
        }
    }

    myfile.close();

    // Details and code on PPM files available at:
    // https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations/reading-writing-images.html
    
    return true;
}
