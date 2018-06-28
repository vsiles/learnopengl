#include <fstream>
#include <iostream>
#include "shader.hpp"

using namespace vinz;
using namespace std;

Shader::Shader(const vector<char> &shader, Shader::Type type)
{
    ShaderFailure cause;
    if (!from_bytes(shader, type, cause))
        throw cause;
}

Shader::Shader(const string &filename, Shader::Type type)
{
    ifstream fp;

    fp.open(filename.c_str(), ios_base::in);

    if (!fp.is_open() || !fp.good())
        throw ShaderFailure(ShaderFailure::Cause::IO);
    fp.exceptions(ifstream::failbit | ifstream::badbit | ifstream::eofbit);
    try {
        fp.seekg(0, ios_base::end);
        size_t size = fp.tellg();
        fp.seekg(0, ios_base::beg);

        vector<char> shader(size);
        fp.read(&(shader[0]), size);
        fp.close();
        ShaderFailure cause;
        if (!from_bytes(shader, type, cause))
            throw cause;
    } catch (ifstream::failure &) {
        throw ShaderFailure(ShaderFailure::Cause::IO);
    }
}

Shader::~Shader()
{
    glDeleteShader(id);
}

bool Shader::from_bytes(const vector<char> &shader, Shader::Type type,
                        ShaderFailure &cause)
{
    if (id != 0) {
        glDeleteShader(id);
    }

    GLenum stype;
    switch (type) {
        case Shader::Type::Vertex:
            stype = GL_VERTEX_SHADER;
            break;
        case Shader::Type::Fragment:
            stype = GL_FRAGMENT_SHADER;
            break;
    }

    id = glCreateShader(stype);
    if (id == 0) {
        cause = ShaderFailure::Cause::Create;
        return false;
    }

    const GLchar *shader_source = static_cast<const GLchar *>(&(shader[0]));
    GLint shader_length = shader.size();
    glShaderSource(id, 1, &shader_source, &shader_length);
    glCompileShader(id);
    GLint ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (ok == 0) {
        cause = ShaderFailure::Cause::Compile;
        glGetShaderInfoLog(id, 512, nullptr, cause.log);
        return false;
    }
    return true;
}

bool ShaderProgram::init(const Shader &vert, const Shader &frag,
                         ShaderFailure &cause)
{
    if (id != 0)
        glDeleteProgram(id);

    id = glCreateProgram();
    if (id == 0) {
        cause = ShaderFailure::Cause::Create;
        return false;
    }

    glAttachShader(id, vert.id);
    glAttachShader(id, frag.id);
    glLinkProgram(id);

    GLint ok;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (ok == 0) {
        cause = ShaderFailure::Cause::Compile;
        glGetProgramInfoLog(id, 512, nullptr, cause.log);
        return false;
    }
    return true;
}

ShaderProgram::~ShaderProgram()
{
    if (id != 0) {
        glDeleteProgram(id);
    }
}

void ShaderProgram::activate()
{
    glUseProgram(id);
}
