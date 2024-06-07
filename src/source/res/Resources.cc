#include "../../headers/res/Resources.h"

namespace res
{

std::map<string, s_ptr<Model>> models;
std::map<std::vector<string>, s_ptr<Shader>> shaders;
std::map<string, s_ptr<tmp::Texture>> textures;
std::map<string, s_ptr<Font>> fonts;

FT_Library ft;

s_ptr<Model> get_model(string path)
{
    if (models.contains(path))
    {
        return models[path];
    }
    s_ptr<Model> model = make_shared<Model>(path);
    models[path] = model;
    return model;
}

s_ptr<Shader> get_shader(string v_path, string f_path)
{
    if (shaders.contains({v_path, f_path}))
    {
        return shaders[{v_path, f_path}];
    }
    s_ptr<Shader> shader = make_shared<Shader>(v_path, f_path);
    shaders[{v_path, f_path}] = shader;
    return shader;
}

s_ptr<Shader> get_shader(string v_path, string g_path, string f_path)
{
    if (shaders.contains({v_path, g_path, f_path}))
    {
        return shaders[{v_path, g_path, f_path}];
    }
    s_ptr<Shader> shader = make_shared<Shader>(v_path, g_path, f_path);
    shaders[{v_path, g_path, f_path}] = shader;
    return shader;
}

s_ptr<tmp::Texture> get_texture(string path)
{
    if (textures.contains(path))
    {
        return textures[path];
    }
    s_ptr<tmp::Texture> texture = make_shared<tmp::Texture>(path);
    textures[path] = texture;
    return texture;
}

s_ptr<Font> get_font(string path)
{
    if (fonts.contains(path))
    {
        return fonts[path];
    }
    s_ptr<Font> font = make_shared<Font>(ft, path.c_str());
    fonts[path] = font;
    return font;
}

int init_freetype()
{
    if (FT_Init_FreeType(&ft))
    {
        cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
        return -1;
    }
    return 0;
}

} // namespace res