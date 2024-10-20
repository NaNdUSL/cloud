#include "ImagePFM.hpp"
#include <iostream>
#include <fstream>
#include <vector>

void ImagePFM::ToneMap() {

    imageToSave = new PFM_pixel[this->W*this->H];

    for (int j = 0 ; j< H ; j++) {

        for (int i = 0; i < W ; ++i) {

            imageToSave[j*W+i].val[0] = std::min(1.f, imagePlane[j*W+i].R);
            imageToSave[j*W+i].val[1] = std::min(1.f, imagePlane[j*W+i].G);
            imageToSave[j*W+i].val[2] = std::min(1.f, imagePlane[j*W+i].B);
        }
    }
}

bool ImagePFM::Save(std::string filename) {
    ToneMap();

    std::ofstream outfile(filename.c_str(), std::ios::binary);

    if (!outfile) {
        std::cerr << "Error opening output PFM file." << std::endl;
        return false;
    }

    outfile << "PF\n";
    outfile << this->W << " " << this->H << "\n";
    outfile << "-1.0\n";

    for (int j = H - 1; j >= 0; j--) {  // Iterate in reverse order
        for (int i = 0; i < W; i++) {

            outfile.write((const char*)(&imageToSave[j*W+i].val[0]), sizeof(float));
            outfile.write((const char*)(&imageToSave[j*W+i].val[1]), sizeof(float));
            outfile.write((const char*)(&imageToSave[j*W+i].val[2]), sizeof(float));
        }
    }

    outfile.close();
    return true;
}
