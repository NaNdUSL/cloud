//
//  ImageJPG.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#ifndef ImageJPG_hpp
#define ImageJPG_hpp
#include "image.hpp"
#include <jpeglib.h>

#include <string>

class ImageJPG: public Image {
    typedef struct {
        unsigned char val[3];  // r,g,b
    } JPG_pixel;
    JPG_pixel *imageToSave;
    void ToneMap ();
public:
    ImageJPG(const int W, const int H):Image(W, H) {}
    bool Save (std::string filename) override;
};

#endif /* ImageJPG_hpp */
