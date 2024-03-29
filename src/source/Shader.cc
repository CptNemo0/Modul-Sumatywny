#include "../headers/Shader.h"

Shader::Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
{
    // Zmienne z kodem zrodlowym shaderow
    std::string v;
    std::string f;

    std::ifstream vertexReader;
    std::ifstream fragmtReader;

    try
    {
        std::stringstream vertexStream;
        std::stringstream fragmtStream;

        // Otworzenie plikow
        vertexReader.open(vertex_shader_path);
        fragmtReader.open(fragment_shader_path);

        // Odczyt
        vertexStream << vertexReader.rdbuf();
        fragmtStream << fragmtReader.rdbuf();

        vertexReader.close();
        fragmtReader.close();

        v = vertexStream.str();
        f = fragmtStream.str();
    }
    catch (std::fstream::failure e)
    {
        printf("You have f'ed up!");
    }

    const char* vertexSource = v.c_str();
    const char* fragmtSource = f.c_str();

    unsigned int vertex;
    unsigned int fragmt;

    int success;
    char* infoLog = new char[512];

    // Kompilacja shaderow
    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Vertex shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Vertex shader compiled\n");
    }

    // Fragment shader
    fragmt = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmt, 1, &fragmtSource, NULL);
    glCompileShader(fragmt);
    // check for shader compile errors
    glGetShaderiv(fragmt, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmt, 512, NULL, infoLog);
        printf("Fragment shader compilation failed\n");
        printf(infoLog);
        exit(1);
    }
    else
    {
        printf("Fragment shader compiled\n");
    }

    // Stworzenie programu wykonujacego shadery
    id_ = glCreateProgram();     // Tworzenie programy shaderow
    glAttachShader(id_, vertex); // Przyczepienie vertex shaderu do dzielonego programu
    glAttachShader(id_, fragmt); // Przyczepienie fragment shaderu do dzielonego programu
    glLinkProgram(id_);          // Utworzenie ostatecznego obiektu programu polaczonego z shaderami

    glDeleteShader(vertex);
    glDeleteShader(fragmt);
}

Shader::~Shader()
{
    End();
}

void Shader::Use()
{
    glUseProgram(id_);
}

void Shader::End()
{
    glDeleteProgram(id_);
}

unsigned int Shader::get_id() const
{
    return id_;
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}
void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}
void Shader::SetMatrix4(const std::string& name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(id_, name.c_str()), value.x, value.y, value.z);

}

void Shader::SetLight(const std::string& name, Light value)
{
    std::string intensity_name = name + ".intensity";
    std::string ambient_colour_name = name + ".ambient_colour";
    std::string diffuse_colour_name = name + ".diffuse_colour";
    std::string specular_colour_name = name + ".specular_colour";

    SetFloat(intensity_name, value.intensity);
    SetVec3(ambient_colour_name, value.ambient_colour);
    SetVec3(diffuse_colour_name, value.diffuse_colour);
    SetVec3(specular_colour_name, value.specular_colour);
}

void Shader::SetDirectionalLight(const std::string& name, DirectionalLight value)
{
    SetLight(name, value);

    std::string direction_name = name + ".direction";
    SetVec3(direction_name, value.direction);
}

void Shader::SetPointLight(const std::string& name, PointLight value)
{
    SetLight(name, value);

    std::string position_name = name + ".position";
    SetVec3(position_name, value.position);
}

void Shader::SetSpotLight(const std::string& name, SpotLight value)
{
    SetLight(name, value);

    std::string position_name = name + ".position";
    std::string direction_name = name + ".direction";
    std::string cut_off_name = name + ".cut_off";

    SetVec3(position_name, value.position);
    SetVec3(direction_name, value.direction);
    SetFloat(cut_off_name, value.cut_off);    
}