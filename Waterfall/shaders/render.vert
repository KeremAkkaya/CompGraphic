#version 330

layout (location = 2)  in vec3  positionIn;
layout (location = 5)  in vec3  colorIn;
layout (location = 6)  in float fullLifeTimeIn;
layout (location = 7)  in float actualLifeTimeIn;
layout (location = 8)  in float sizeIn;
layout (location = 11) in float opacityIn;

out vec3  color;
out float fullLifeTime;
out float actualLifeTime;
out float size;
out float opacity;

void main()
{
    gl_Position    = vec4(positionIn, 1.0f);
    color          = colorIn;
    fullLifeTime   = fullLifeTimeIn;
    actualLifeTime = actualLifeTimeIn;
    size           = sizeIn;
    opacity        = opacityIn;
}
