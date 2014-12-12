#include "particlesystem.h"
#include "common.h"
#include "utils.h"

size_t Particle::serializedSize()
{
    static const size_t serializedSize =
        sizeof(GLfloat)    // randInit
        + sizeof(vec3)     // positionInit
        + sizeof(vec3)     // position
        + sizeof(vec3)     // velocityInit
        + sizeof(vec3)     // velocity
        + sizeof(vec3)     // color
        + sizeof(GLfloat)  // fullLifeTime
        + sizeof(GLfloat)  // actualLifeTime
        + sizeof(GLfloat)  // size
        + sizeof(GLfloat)  // minSize
        + sizeof(GLfloat)  // maxSize
        + sizeof(GLfloat)  // opacity
        ;

    return serializedSize;
}

size_t Particle::serialize(GLfloat* buf)
{
    size_t index = 0;

    index += serializeGLfloat(buf + index, randInit);
    index += serializeVec3(buf + index, positionInit);
    index += serializeVec3(buf + index, position);
    index += serializeVec3(buf + index, velocityInit);
    index += serializeVec3(buf + index, velocity);
    index += serializeVec3(buf + index, color);
    index += serializeGLfloat(buf + index, fullLifeTime);
    index += serializeGLfloat(buf + index, actualLifeTime);
    index += serializeGLfloat(buf + index, size);
    index += serializeGLfloat(buf + index, minSize);
    index += serializeGLfloat(buf + index, maxSize);
    index += serializeGLfloat(buf + index, opacity);

    assert(index == PARTICLE_SERIALIZED_GLFLOAT_COUNT);
    return index;
}

ParticleSystem::ParticleSystem()
    : _isInitialized(false)
    , _maxParticlesCount(0)
    , _particlesDataSize(0)
    , _particlesData(NULL)
{
}

ParticleSystem::~ParticleSystem()
{
    if (_particlesData != NULL) {
        delete _particlesData;
    }
}

void ParticleSystem::loadTextureAtlas(string const& fileName, size_t rowCount, size_t columnCount)
{
    _texture.loadTexture(fileName, false, rowCount, columnCount);
    _texture.bindTexture(0);
    _texture.setFiltering(TEXTURE_FILTER_MAG_LINEAR, TEXTURE_FILTER_MIN_LINEAR);
}

void ParticleSystem::generateParticles()
{
    if (_maxParticlesCount <= 0) {
        throw std::runtime_error("Try to generate non-positive count of particles...");
    }

    _particlesDataSize = PARTICLE_SERIALIZED_GLFLOAT_COUNT * _maxParticlesCount;
    _particlesData     = new GLfloat[_particlesDataSize];

    size_t offset = 0;
    int RAND_PRECISION = RAND_MAX;
    for (size_t p = 0; p < _maxParticlesCount; ++p) {
        Particle particle;
        particle.randInit = getRandomRange(-1, 1, RAND_PRECISION);
        particle.positionInit = getRandomValueVicinityVec3(emitterPosition, emitterVicinity, RAND_PRECISION);
        particle.position = vec3(0.0f, 0.0f, 0.0f);
        particle.velocityInit = getRandomValueVicinityVec3(averageVelocity, velocityVicinity, RAND_PRECISION);
        particle.velocity = vec3(0.0f, 0.0f, 0.0f);
        particle.color = colorInit;// * getRandom01Vec3(RAND_PRECISION);
        particle.fullLifeTime = getRandomRange(minLifeTime, maxLifeTime, RAND_PRECISION);
        particle.actualLifeTime = particle.fullLifeTime * getRandom01(RAND_PRECISION);
        particle.size = 0;
        particle.minSize = minSize + (maxSize - minSize) * getRandomRange(0, 0.5, RAND_PRECISION);
        particle.maxSize = maxSize + (maxSize - minSize) * getRandomRange(0, 0.5, RAND_PRECISION);
        particle.opacity = 0;

        offset += particle.serialize(_particlesData + offset);
    }

    assert(offset == _particlesDataSize);
}

void ParticleSystem::initialize(size_t particlesCount)
{
    if (_isInitialized) {
        return;
    }

    _maxParticlesCount = particlesCount;
    generateParticles();

    const char* varyings[PARTICLE_ATTRIBUTES_COUNT] = {
        "randInitOut",
        "positionInitOut", "positionOut",
        "velocityInitOut", "velocityOut",
        "colorOut",
        "fullLifeTimeOut",
        "actualLifeTimeOut",
        "sizeOut", "minSizeOut", "maxSizeOut",
        "opacityOut"
    };

    _vertShaderUpdate.createShader(GL_VERTEX_SHADER, "shaders//update.vert");
    _geomShaderUpdate.createShader(GL_GEOMETRY_SHADER, "shaders//update.geom");

    _programUpdate.createProgram();
    _programUpdate.addShader(&_vertShaderUpdate);
    _programUpdate.addShader(&_geomShaderUpdate);
    for (size_t i = 0; i < PARTICLE_ATTRIBUTES_COUNT; ++i) {
        glTransformFeedbackVaryings(_programUpdate.getProgramId(), PARTICLE_ATTRIBUTES_COUNT, varyings, GL_INTERLEAVED_ATTRIBS);
    }
    _programUpdate.linkProgram();

    _vertShaderRender.createShader(GL_VERTEX_SHADER, "shaders//render.vert");
    _geomShaderRender.createShader(GL_GEOMETRY_SHADER, "shaders//render.geom");
    _fragShaderRender.createShader(GL_FRAGMENT_SHADER, "shaders//render.frag");

    _programRender.createProgram();
    _programRender.addShader(&_vertShaderRender);
    _programRender.addShader(&_geomShaderRender);
    _programRender.addShader(&_fragShaderRender);
    _programRender.linkProgram();

    glGenTransformFeedbacks(1, &_transformFeedbackBuffer);
    glGenBuffers(2, _particlesBuffers);
    glGenVertexArrays(2, _VAOs);

    for (size_t i = 0; i < 2; ++i) {
        glBindVertexArray(_VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, _particlesBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER, _particlesDataSize * sizeof(GLfloat), _particlesData, GL_DYNAMIC_DRAW);
        
        for (size_t i = 0; i < PARTICLE_ATTRIBUTES_COUNT; ++i) {
            glEnableVertexAttribArray(i);
        }

        glVertexAttribPointer(0,  1, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(0  * sizeof(GLfloat)));  //randInit
        glVertexAttribPointer(1,  3, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(1  * sizeof(GLfloat)));  //positionInit
        glVertexAttribPointer(2,  3, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(4  * sizeof(GLfloat)));  //position
        glVertexAttribPointer(3,  3, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(7  * sizeof(GLfloat)));  //velocityInit
        glVertexAttribPointer(4,  3, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(10 * sizeof(GLfloat)));  //velocity
        glVertexAttribPointer(5,  3, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(13 * sizeof(GLfloat)));  //color
        glVertexAttribPointer(6,  1, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(16 * sizeof(GLfloat)));  //fullLifeTime
        glVertexAttribPointer(7,  1, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(17 * sizeof(GLfloat)));  //actualLifeTime
        glVertexAttribPointer(8,  1, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(18 * sizeof(GLfloat)));  //size
        glVertexAttribPointer(9,  1, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(19 * sizeof(GLfloat)));  //minSize
        glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(20 * sizeof(GLfloat)));  //maxSize
        glVertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, Particle::serializedSize(), (const GLvoid*)(21 * sizeof(GLfloat)));  //opacity

        glBindVertexArray(0);
    }

    _curReadBuffer = 0;
    _isInitialized = true;
}

void ParticleSystem::updateParticles(float timePassed)
{
    if (!_isInitialized) {
        return;
    }

    _programUpdate.useProgram();
    _programUpdate.setUniform("timePassed", timePassed);
    _programUpdate.setUniform("gravity",    gravity);

    glEnable(GL_RASTERIZER_DISCARD);

    GLuint query;
    glGenQueries(1, &query);

    glBindVertexArray(_VAOs[_curReadBuffer]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(9);
    glEnableVertexAttribArray(10);
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _transformFeedbackBuffer);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _particlesBuffers[1 - _curReadBuffer]);
    
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
    glBeginTransformFeedback(GL_POINTS);

    glDrawArrays(GL_POINTS, 0, _maxParticlesCount);

    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    
    _curReadBuffer = 1 - _curReadBuffer;

    GLuint primitivesCount;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitivesCount);
    assert(primitivesCount == _maxParticlesCount);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, _particlesBuffers[1 - _curReadBuffer]);
    //GLfloat data[22];
    //glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
    //int i = 0;
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::renderParticles()
{
    if (!_isInitialized) {
        return;
    }

    _programRender.useProgram();

    glDisable(GL_RASTERIZER_DISCARD);
    
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    _programRender.setUniform("texRowCount", _texture.rowCount());
    _programRender.setUniform("texColumnCount", _texture.columnCount());
    _programRender.setUniform("mView", mView);
    _programRender.setUniform("mProj", mProj);
    _programRender.setUniform("quad1", _quad1);
    _programRender.setUniform("quad2", _quad2);
    _programRender.setUniform("tSampler", _texture.textureUnit());

    glBindVertexArray(_VAOs[_curReadBuffer]);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(9);
    glDisableVertexAttribArray(10);

    glDrawArrays(GL_POINTS, 0, _maxParticlesCount);

    glDisable(GL_BLEND);
    glDepthMask(1);
}

void ParticleSystem::setMaxParticlesCount(int maxParticlesCount)
{
    _maxParticlesCount = maxParticlesCount;
}

void ParticleSystem::setMatrices(mat4 mProj, vec3 eye, vec3 viewCenter, vec3 upVector, quat rotation)
{
    this->mProj = mProj;
    mView = lookAt(eye, viewCenter, upVector); // * mat4_cast(rotation);

    vec3 viewDirection = viewCenter - eye;
    _quad1 = cross(viewDirection, upVector);
    _quad2 = cross(_quad1, viewDirection);
    _quad1 = normalize(_quad1);
    _quad2 = normalize(_quad2);

    assert(_quad1 == vec3(1.0f, 0.0f, 0.0f));
    assert(_quad2 == vec3(0.0f, 1.0f, 0.0f));
}

