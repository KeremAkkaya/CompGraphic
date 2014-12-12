#include "texture.h"

TextureAtlas::TextureAtlas()
    : _textureUnit(-1), _mipmapGenerated(false), _magFilter(NO_TEXTURE_FILTER), _minFilter(NO_TEXTURE_FILTER), _textureFileName(""), _rowCount(0), _columnCount(0)
{}

bool TextureAtlas::loadTexture(const string& textureFileName, bool mipmapRequired, int rowCount, int columnCount)
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP* dib = NULL;

    fif = FreeImage_GetFileType(textureFileName.c_str(), 0);

    if (fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(textureFileName.c_str());
    }

    if (fif == FIF_UNKNOWN) {
        throw std::runtime_error("Unknown image format: " + textureFileName);
        return false;
    }

    if (FreeImage_FIFSupportsReading(fif)) {
        dib = FreeImage_Load(fif, textureFileName.c_str());
    }

    if (!dib) {
        throw std::runtime_error("Can't load image: " + textureFileName);
        return false;
    }

    FIBITMAP* temp = dib;
    dib = FreeImage_ConvertTo32Bits(dib);
    FreeImage_Unload(temp);

    BYTE* data = FreeImage_GetBits(dib);
    _width = FreeImage_GetWidth(dib);
    _height = FreeImage_GetHeight(dib);
    _bpp = FreeImage_GetBPP(dib);

    if (data == NULL || _width == 0 || _height == 0) {
        throw std::runtime_error("Can't retrieve info from image: " + textureFileName);
        return false;
    }

    GLubyte* textura = new GLubyte[4 * _width*_height];
    char* pixeles = (char*)FreeImage_GetBits(dib);
    //FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).

    for (int j = 0; j<_width *_height; j++){
        textura[j * 4 + 0] = pixeles[j * 4 + 2];
        textura[j * 4 + 1] = pixeles[j * 4 + 1];
        textura[j * 4 + 2] = pixeles[j * 4 + 0];
        textura[j * 4 + 3] = pixeles[j * 4 + 3];
        //cout<<j<<": "<<textura[j*4+0]<<"**"<<textura[j*4+1]<<"**"<<textura[j*4+2]<<"**"<<textura[j*4+3]<<endl;
    }

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)textura);

    if (mipmapRequired) {
        glGenerateMipmap(GL_TEXTURE_2D);
        _mipmapGenerated = true;
    }
    
    FreeImage_Unload(dib);
    glGenSamplers(1, &_sampler);

    _textureUnit = 0;
    _textureFileName = textureFileName;
    _rowCount = rowCount;
    _columnCount = columnCount;

    return true;
}

int TextureAtlas::rowCount()
{
    return _rowCount;
}

int TextureAtlas::columnCount()
{
    return _columnCount;
}

void TextureAtlas::bindTexture(int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glBindSampler(textureUnit, _sampler);
    _textureUnit = textureUnit;
}

int TextureAtlas::textureUnit()
{
    return _textureUnit;
}

void TextureAtlas::setFiltering(int magFilter, int minFilter)
{
    switch (magFilter) {
    case TEXTURE_FILTER_MAG_NEAREST:
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TEXTURE_FILTER_MAG_LINEAR:
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }

    switch (minFilter) {
    case TEXTURE_FILTER_MIN_NEAREST:
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    case TEXTURE_FILTER_MIN_LINEAR:
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case TEXTURE_FILTER_MIN_NEAREST_MIPMAP_NEAREST:
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        break;
    case TEXTURE_FILTER_MIN_NEAREST_MIPMAP_LINEAR:
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        break;
    case TEXTURE_FILTER_MIN_LINEAR_MIPMAP_NEAREST:
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    case TEXTURE_FILTER_MIN_LINEAR_MIPMAP_LINEAR:
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
}

void TextureAtlas::releaseTexture()
{
    glDeleteSamplers(1, &_sampler);
    glDeleteTextures(1, &_texture);
}
