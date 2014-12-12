#ifndef UTILS_H
#define UTILS_H

#include "common.h"
#include "utils.h"


float getRandom01(int precision);
float getRandomRange(float left, float right, int precision);
float getRandomValueVicinity(float value, float vicinity, int precision);

vec3 getRandom01Vec3(int precision);
vec3 getRandomRangeVec3(vec3 left, vec3 right, int precision);
vec3 getRandomValueVicinityVec3(vec3 value, vec3 vicinity, int precision);

size_t serializeGLfloat(GLfloat* buf, GLfloat value);
size_t serializeVec3(GLfloat* buf, vec3 v);

#endif //UTILS_H
