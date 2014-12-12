#ifndef SHADERS_H
#define SHADERS_H

#include "common.h"

class WShader
{
    GLuint _shader;
    GLenum _type;
    bool _isCompiled;

public:
    WShader();

    bool createShader(GLenum type, const string& fileName);
    void deleteShader();

    bool isCompiled();
    GLuint getShaderId();
};

class WProgram
{
    GLuint _program;
    bool _isLinked;

public:
    WProgram(); 

    void createProgram();
    void deleteProgram();

    bool addShader(WShader* wshader);
    bool linkProgram();

    void useProgram();

    GLuint getProgramId();

    // Setting integers
    void setUniform(const string& name, int count, int* values);
    void setUniform(const string& name, int value);

    // Setting floats
    void setUniform(const string& name, int count, float* values);
    void setUniform(const string& name, float value);

    // Setting vectors
    void setUniform(const string& name, int count, vec2* vectors);
    void setUniform(const string& name, vec2 vector);
    void setUniform(const string& name, int count, vec3* vectors);
    void setUniform(const string& name, vec3 vector);
    void setUniform(const string& name, int count, vec4* vectors);
    void setUniform(const string& name, vec4 vector);

    // Setting 3x3 matrices
    void setUniform(const string& name, int count, mat3* matrices);
    void setUniform(const string& name, mat3 matrix);

    // Setting 4x4 matrices
    void setUniform(const string& name, int count, mat4* matrices);
    void setUniform(const string& name, mat4 matrix);
};

#endif //SHADERS_H