#version 330

layout (location = 0)  in float randInitIn;
layout (location = 1)  in vec3  positionInitIn; layout (location = 2) in vec3 positionIn;
layout (location = 3)  in vec3  velocityInitIn; layout (location = 4) in vec3 velocityIn;
layout (location = 5)  in vec3  colorIn;
layout (location = 6)  in float fullLifeTimeIn;
layout (location = 7)  in float actualLifeTimeIn;
layout (location = 8)  in float sizeIn; layout (location = 9) in float minSizeIn; layout (location = 10) in float maxSizeIn;
layout (location = 11) in float opacityIn;

out float randInit;
out vec3  positionInit, position;
out vec3  velocityInit, velocity;
out vec3  color;
out float fullLifeTime;
out float actualLifeTime;
out float size, minSize, maxSize;
out float opacity;

void main()
{
    randInit       = randInitIn;
    positionInit   = positionInitIn; position = positionIn;
    velocityInit   = velocityInitIn; velocity = velocityIn;
    color          = colorIn;
    fullLifeTime   = fullLifeTimeIn;
    actualLifeTime = actualLifeTimeIn;
    size           = sizeIn; minSize = minSizeIn; maxSize = maxSizeIn;
    opacity        = opacityIn;
}
