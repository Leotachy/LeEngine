
#include "llShader.h"




llShader::llShader(const char* vertexPath, const char* fragmentPath)
{


    int vertex = compileShader(GL_VERTEX_SHADER, vertexPath);
    int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentPath);

    int success;
    char infoLog[512];

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);


}


void llShader::use() const
{
    glUseProgram(ID);
}

void  llShader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void  llShader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void  llShader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void llShader::SetVec3(const std::string& name, const float* values) const
{
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform3f(loc, values[0], values[1], values[2]);
}

void llShader::SetMat4(const std::string& name, const float* values) const
{
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, values);
}

// private methods



int llShader::compileShader(unsigned int shaderType, const char* shaderPath)
{

    // 1. retrieve the vertex/fragment source code from filePath
    std::string code;
    std::ifstream file;
    // ensure ifstream objects can throw exceptions:
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        file.open(shaderPath);
        std::stringstream stream;
        // read file's buffer contents into streams
        stream << file.rdbuf();
        // close file handlers
        file.close();
        // convert stream into string
        code = stream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* shaderSource = code.c_str();

    // 2. compile shaders
    unsigned int shaderId;
    int success;
    char infoLog[512];

    // vertex Shader
    shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSource, NULL);
    glCompileShader(shaderId);
    // print compile errors if any
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return -1;
    };

    return int(shaderId);
}