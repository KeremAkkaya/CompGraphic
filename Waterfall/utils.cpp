#include "utils.h"


float getRandom01(int precision)
{
    return 1.0f * (rand() % precision) / precision;
}

float getRandomRange(float left, float right, int precision) 
{
    float r = left < right ? getRandom01(precision) : 1 - getRandom01(precision);

    return left + r * (right - left);
}

float getRandomValueVicinity(float value, float vicinity, int precision)
{
    return getRandomRange(value - vicinity, value + vicinity, precision);
}

vec3 getRandom01Vec3(int precision)
{
    return vec3(
        getRandom01(precision),
        getRandom01(precision),
        getRandom01(precision)
        );
}

vec3 getRandomRangeVec3(vec3 left, vec3 right, int precision)
{
    return vec3(
        getRandomRange(left.x, right.x, precision),
        getRandomRange(left.y, right.y, precision),
        getRandomRange(left.z, right.z, precision)
        );
}

vec3 getRandomValueVicinityVec3(vec3 value, vec3 vicinity, int precision) {
    return vec3(
        getRandomValueVicinity(value.x, vicinity.x, precision),
        getRandomValueVicinity(value.y, vicinity.y, precision),
        getRandomValueVicinity(value.z, vicinity.z, precision)
        );
}

size_t serializeGLfloat(GLfloat* buf, GLfloat value)
{
    *(buf) = value;

    return 1;
}

size_t serializeVec3(GLfloat* buf, vec3 v)
{
    for (size_t i = 0; i < 3; ++i) {
        GLfloat value = v[i];
        *(buf + i) = value;
    }

    return 3;
}