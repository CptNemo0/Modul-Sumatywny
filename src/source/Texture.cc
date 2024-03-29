#include "../headers/Texture.h"

Texture::Texture(const std::string& path)
{
    id_ = 0;
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int t_width, t_height, t_c;
    unsigned char* img = stbi_load(path.c_str(), &t_width, &t_height, &t_c, 0);
    if (img)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture loaded.\n";
    }
    else
    {
        std::cout << "Failed to load a texture!!!\n";
    }

    stbi_image_free(img);
    glBindTexture(GL_TEXTURE_2D, 0);
}
