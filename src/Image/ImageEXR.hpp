//
//  ImageEXR.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#ifndef ImageEXR_hpp
#define ImageEXR_hpp
#include "image.hpp"
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfRgbaFile.h>

#include <string>

class ImageEXR: public Image {
    typedef struct {
        float val[3];  // r,g,b
    } EXR_pixel;
    EXR_pixel *imageToSave;
    void ToneMap ();
public:
    ImageEXR(const int W, const int H):Image(W, H) {}
    bool Save (std::string filename) override;
};

#endif /* ImageEXR_hpp */
