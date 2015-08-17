#ifndef G_BUFFER_H
#define G_BUFFER_H

#include <GL/glew.h>

class GBuffer
{
public:

    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_AMBIENT,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_SPECULAR,
        GBUFFER_TEXTURE_TYPE_EMISSIVE,
        GBUFFER_NUM_TEXTURES,
    };

    GBuffer();
    ~GBuffer();
	
    bool Init(unsigned width, unsigned height);
    void BindForWriting();
    void BindForReading(GLenum textureUnit, unsigned type);
	GLuint GetTexture(GBUFFER_TEXTURE_TYPE type) {return m_textures[type];}

//private:

    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_depthTexture;
	unsigned m_width;
	unsigned m_height;
};

#endif