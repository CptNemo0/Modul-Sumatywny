#ifndef TEXTURE_H
#define TEXTURE_H

#include "string"
#include "stb_image.h"

struct Texture {
    unsigned int id_;
    std::string type_;
    std::string path_;
};

#endif //TEXTURE_H

