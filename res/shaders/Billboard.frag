#version 330 core

in vec2 og_texcoord;

out vec4 of_color;

uniform sampler2D u_texture;

void main()
{
    of_color = texture(u_texture, og_texcoord);
}