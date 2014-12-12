#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "common.h"
#include "shaders.h"
#include "texture.h"

static const size_t PARTICLE_ATTRIBUTES_COUNT = 12;
static const size_t PARTICLE_SERIALIZED_GLFLOAT_COUNT = 22;

struct Particle
{
    GLfloat randInit;
    vec3 positionInit, position;
    vec3 velocityInit, velocity;
    vec3 color;
    GLfloat fullLifeTime;
    GLfloat actualLifeTime;
    GLfloat size, maxSize, minSize;
    GLfloat opacity;

    static size_t serializedSize();
    size_t serialize(GLfloat* buf);
};

class ParticleSystem
{
    bool _isInitialized;

    size_t _maxParticlesCount;
    size_t _particlesDataSize;
    GLfloat* _particlesData;

    WShader _vertShaderUpdate, _geomShaderUpdate;
    WProgram _programUpdate;

    WShader _vertShaderRender, _geomShaderRender, _fragShaderRender;
    WProgram _programRender;

    size_t _curReadBuffer;
    GLuint _transformFeedbackBuffer;
    GLuint _particlesBuffers[2];
    GLuint _VAOs[2];

    vec3 _quad1, _quad2;
    TextureAtlas _texture;

    void generateParticles();

public:
    vec3  emitterPosition, emitterVicinity;
    vec3  averageVelocity, velocityVicinity;
    vec3  gravity;
    float minLifeTime, maxLifeTime;
    float minSize, maxSize;
    vec3  colorInit;
    float opacityInit;
    mat4 mView, mProj;

    ParticleSystem();
    ~ParticleSystem();

    void initialize(size_t particlesCount);
    void loadTextureAtlas(string const& fileName, size_t rowCount, size_t columnCount);
    
    void setMaxParticlesCount(int maxParticlesCount);
    void setMatrices(mat4 mProj, vec3 camera, vec3 view, vec3 upVector, quat rotation);

    void updateParticles(float timePassed);
    void renderParticles();
};

#endif //PARTICLE_SYSTEM_H
