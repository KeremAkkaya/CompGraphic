#ifndef WATERFALL_PROGRAM_H
#define WATERFALL_PROGRAM_H

#include "common.h"
#include "particlesystem.h"

class WaterfallProgram
{
    chrono::system_clock::time_point _lastFrameTP;

    float _cameraZPosition;
    float _cameraFOV;
    quat _rotation;

    vec3 _emitterPosition;
    vec3 _emitterVicinity;
    vec3 _averageVelocity;
    vec3 _velocityVicinity;
    vec3 _gravity;
    float _minLifeTime;
    float _maxLifeTime;
    float _minSize;
    float _maxSize;

    vec3 _particleColor;
    float _particleOpacity;

    ParticleSystem _particleSystem;

    void initSettings();
    void initAntTweakBar();
    void initParticleSystem();
    void setupParticleSystem();

public:
    WaterfallProgram();

    void drawFrame();
    float updateTimer();
};

#endif //WATERFALL_PROGRAM_H