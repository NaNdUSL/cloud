//
//  ImagePFM.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#ifndef ImagePFM_hpp
#define ImagePFM_hpp
#include "image.hpp"

#include <string>

class ImagePFM: public Image {
    typedef struct {
        float val[3];  // r,g,b
    } PFM_pixel;
    PFM_pixel *imageToSave;
    void ToneMap ();
public:
    ImagePFM(const int W, const int H):Image(W, H) {}
    bool Save (std::string filename) override;
};

#endif /* ImagePFM_hpp */
