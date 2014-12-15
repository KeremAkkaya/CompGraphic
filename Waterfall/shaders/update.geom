#version 330

layout (points) in;
layout (points, max_vertices = 1) out;

in float randInit[];
in vec3  positionInit[], position[];
in vec3  velocityInit[], velocity[];
in vec3  color[];
in float fullLifeTime[];
in float actualLifeTime[];
in float size[], minSize[], maxSize[];
in float opacity[];

out float randInitOut;
out vec3  positionInitOut, positionOut;
out vec3  velocityInitOut, velocityOut;
out vec3  colorOut;
out float fullLifeTimeOut;
out float actualLifeTimeOut;
out float sizeOut, minSizeOut, maxSizeOut;
out float opacityOut;

uniform float timePassed;
uniform vec3  gravity;

vec3 localSeed;

float computeOpacity(float relativeLifeTime)
{
    if (relativeLifeTime < 0.3) {
        return 0.4 + 2 * relativeLifeTime;
    }
    if (relativeLifeTime < 0.6) {
        return 1.0;
    }
    if (relativeLifeTime < 1) {
        return 2.5 - 2.5 * relativeLifeTime;
    }
}

float computeSize(float relativeLifeTime, float minSize, float maxSize)
{
    return minSize + (maxSize - minSize) * relativeLifeTime;
}

/*magic random function obtained by Internet browsing*/
float random01()
{
    uint n = floatBitsToUint(localSeed.y * 214013.0 + localSeed.x * 2531011.0 + localSeed.z * 141251.0);
    n = n * (n * n * 15731u + 789221u);
    n = (n >> 9u) | 0x3F800000u;

    float res =  2.0 - uintBitsToFloat(n);
    localSeed = vec3(localSeed.x + 147158.0 * res, localSeed.y * res  + 415161.0 *res, localSeed.z + 324154.0 * res);
    return res;
}


void main()
{
    randInitOut = randInit[0];

    fullLifeTimeOut = fullLifeTime[0];
    actualLifeTimeOut = actualLifeTime[0] + timePassed < fullLifeTimeOut ? actualLifeTime[0] + timePassed : 0;
    
    float relativeLifeTime = actualLifeTimeOut / fullLifeTimeOut;

    positionInitOut = positionInit[0];
    velocityInitOut = velocityInit[0];
    positionOut = positionInitOut + velocityInitOut * actualLifeTimeOut + gravity * pow(actualLifeTimeOut, 2) / 2;
    velocityOut = velocityInitOut + gravity * actualLifeTimeOut;
    
    colorOut = color[0];
    minSizeOut = minSize[0]; 
    maxSizeOut = maxSize[0];
    sizeOut = computeSize(relativeLifeTime, minSizeOut, maxSizeOut);
    opacityOut = computeOpacity(relativeLifeTime);

    EmitVertex();
    EndPrimitive();
}
