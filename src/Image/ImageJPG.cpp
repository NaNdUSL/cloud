//
//  ImageJPG.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#include "ImageJPG.hpp"
#include <iostream>
#include <fstream>
#include <vector>

void ImageJPG::ToneMap() {

    imageToSave = new JPG_pixel[this->W*this->H];

    for (int j = 0 ; j < H ; j++) {

        for (int i = 0; i < W ; i++) {

            imageToSave[j*W+i].val[0] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].R) * 255);
            imageToSave[j*W+i].val[1] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].G) * 255);
            imageToSave[j*W+i].val[2] = (unsigned char)(std::min(1.f, imagePlane[j*W+i].B) * 255);
        }
    }
}

bool ImageJPG::Save (std::string filename) {

    ToneMap();

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE* outfile = fopen(filename.c_str(), "wb");

    if (!outfile) {
        std::cerr << "Error opening output JPEG file." << std::endl;
        jpeg_destroy_compress(&cinfo);
        return false;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = this->W;
    cinfo.image_height = this->H;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    //jpeg_set_quality(&cinfo, 90, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    unsigned char *raw_image = (unsigned char*)malloc(cinfo.image_width * cinfo.image_height * 3);

    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; ++i) {
            int index = (j * W + i) * 3;

            raw_image[index] = this->imageToSave[j * W + i].val[0];
            raw_image[index + 1] = this->imageToSave[j * W + i].val[1];
            raw_image[index + 2] = this->imageToSave[j * W + i].val[2];
        }
    }

    JSAMPROW row_pointer[1];

    while( cinfo.next_scanline < cinfo.image_height )
	{
		row_pointer[0] = &raw_image[cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
		jpeg_write_scanlines( &cinfo, row_pointer, 1);
	}

    jpeg_finish_compress(&cinfo);

    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    
    return true;
}
