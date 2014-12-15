#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 color[];
in float fullLifeTime[];
in float actualLifeTime[];
in float size[];
in float opacity[];

out vec4  colorFragIn;
out vec2  texPrevCoord;
out vec2  texNextCoord;
out float texNextSimilarity;

uniform int texRowCount;
uniform int texColumnCount;

uniform mat4 mView;
uniform mat4 mProj;

uniform vec3 quad1;
uniform vec3 quad2;

struct TextureCoords {
    vec2 leftBottom;
    vec2 leftTop;
    vec2 rightBottom;
    vec2 rightTop;
};

TextureCoords getTextureCoords(int texNum)
{
    float texRowNum    = float(texNum / texColumnCount);
    float texColumnNum = float(texNum % texColumnCount);

    TextureCoords texCoords;
    texCoords.leftBottom  = vec2( texColumnNum      / texColumnCount, 1 - (texRowNum + 1) / texRowCount);
    texCoords.leftTop     = vec2( texColumnNum      / texColumnCount, 1 -  texRowNum      / texRowCount);
    texCoords.rightBottom = vec2((texColumnNum + 1) / texColumnCount, 1 - (texRowNum + 1) / texRowCount);
    texCoords.rightTop    = vec2((texColumnNum + 1) / texColumnCount, 1 -  texRowNum      / texRowCount);

    return texCoords;
}

void main()
{
    colorFragIn = vec4(color[0], opacity[0]);
    int texCount = texRowCount * texColumnCount;
    vec3 posCenter = gl_in[0].gl_Position.xyz;
    mat4 mVP = mProj * mView;
    float relativeLifeTime = actualLifeTime[0] / fullLifeTime[0];

    float texNum = relativeLifeTime * texCount;
    int texPrevNum = int(floor(texNum)) % texCount; //textures enumerated from 0 to texCount - 1
    int texNextNum = (texPrevNum + 1) % texCount;
    texNextSimilarity = fract(texNum);

    TextureCoords texPrevCoords = getTextureCoords(texPrevNum);
    TextureCoords texNextCoords = getTextureCoords(texNextNum);

    vec3 posLeftBottom = posCenter + (-quad1 - quad2) * size[0];
    texPrevCoord = texPrevCoords.leftBottom;
    texNextCoord = texNextCoords.leftBottom;
    gl_Position = mVP * vec4(posLeftBottom, 1.0);
    EmitVertex();

    vec3 posLeftTop = posCenter + (-quad1 + quad2) * size[0];
    texPrevCoord = texPrevCoords.leftTop;
    texNextCoord = texNextCoords.leftTop;
    gl_Position = mVP * vec4(posLeftTop, 1.0);
    EmitVertex();

    vec3 posRightBottom = posCenter + (quad1 - quad2) * size[0];
    texPrevCoord = texPrevCoords.rightBottom;
    texNextCoord = texNextCoords.rightBottom;
    gl_Position = mVP * vec4(posRightBottom, 1.0);
    EmitVertex();

    vec3 posRightTop = posCenter + (quad1 + quad2) * size[0];
    texPrevCoord = texPrevCoords.rightTop;
    texNextCoord = texNextCoords.rightTop;
    gl_Position = mVP * vec4(posRightTop, 1.0);
    EmitVertex();

    EndPrimitive();
}
