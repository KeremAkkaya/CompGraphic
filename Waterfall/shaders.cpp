#include "shaders.h"


WShader::WShader()
    : _isCompiled(false)
{}

bool WShader::createShader(GLenum type, const string& fileName)
{
    ifstream fin(fileName.c_str(), std::ios::binary);
    string fileStr((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

    _shader = glCreateShader(type);
    char const* source = fileStr.c_str();
    glShaderSource(_shader, 1, &source, NULL);
    glCompileShader(_shader);

    GLint status;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        int infoLogLength;
        glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) {
            string buffer;
            buffer.resize(infoLogLength);
            glGetShaderInfoLog(_shader, infoLogLength, NULL, &buffer[0]);
            throw std::runtime_error(fileName + ": " + buffer);
        }
    }

    _type = type;
    _isCompiled = status == GL_TRUE;

    return _isCompiled;
}

bool WShader::isCompiled()
{
    return _isCompiled;
}

GLuint WShader::getShaderId()
{
    return _shader;
}

void WShader::deleteShader()
{
    if (!_isCompiled) {
        return;
    }
    
    glDeleteShader(_shader);
    _isCompiled = false;
}

WProgram::WProgram()
    : _isLinked(false)
{}

void WProgram::createProgram()
{
    _program = glCreateProgram();
}

bool WProgram::addShader(WShader* shader)
{
    if (!shader->isCompiled()) {
        return false;
    }

    glAttachShader(_program, shader->getShaderId());
    return true;
}

bool WProgram::linkProgram()
{
    glLinkProgram(_program);

    GLint status;
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (!status) {
        int infoLogLength;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) {
            string buffer;
            buffer.resize(infoLogLength);
            glGetProgramInfoLog(_program, infoLogLength, NULL, &buffer[0]);
            throw std::runtime_error(buffer);
        }
    }

    _isLinked = status == GL_TRUE;
    return _isLinked;
}

void WProgram::deleteProgram()
{
    if (!_isLinked) {
        return;
    }

    glDeleteProgram(_program);
    _isLinked = false;
}

void WProgram::useProgram()
{
    if (_isLinked) {
        glUseProgram(_program);
    }
}

GLuint WProgram::getProgramId()
{
    return _program;
}

// Setting floats
void WProgram::setUniform(const string& name, int count, float* values)
{
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniform1fv(loc, count, values);
}

void WProgram::setUniform(const string& name, float value)
{
    setUniform(name, 1, &value);
}

// Setting vectors
void WProgram::setUniform(const string& name, int count, vec2* vectors)
{
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniform2fv(loc, count, (GLfloat*)vectors);
}

void WProgram::setUniform(const string& name, vec2 vector)
{
    setUniform(name, 1, &vector);
}

void WProgram::setUniform(const string& name, int count, vec3* vectors)
{
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniform3fv(loc, count, (GLfloat*)vectors);
}

void WProgram::setUniform(const string& name, vec3 vector)
{
    setUniform(name, 1, &vector);
}

void WProgram::setUniform(const string& name, int count, vec4* vectors)
{
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniform4fv(loc, count, (GLfloat*)vectors);
}

void WProgram::setUniform(const string& name, vec4 vector)
{
    setUniform(name, 1, &vector);
}

// Setting 3x3 matrices
void WProgram::setUniform(const string& name, int count, mat3* matrices)
{
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniformMatrix3fv(loc, count, FALSE, (GLfloat*)matrices);
}

void WProgram::setUniform(const string& name, mat3 matrix)
{
    setUniform(name, 1, &matrix);
}

// Setting 4x4 matrices
void WProgram::setUniform(const string& name, int count, mat4* matrices)
{
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniformMatrix4fv(loc, count, FALSE, (GLfloat*)matrices);
}

void WProgram::setUniform(const string& name, mat4 matrix)
{
    setUniform(name, 1, &matrix);
}

// Setting integers
void WProgram::setUniform(const string& name, int count, int* values)
{
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniform1iv(loc, count, values);
}

void WProgram::setUniform(const string& name, int value)
{
    setUniform(name, 1, &value);
}
