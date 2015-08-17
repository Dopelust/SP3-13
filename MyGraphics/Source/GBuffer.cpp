#include "GBuffer.h"
#include <iostream>

GBuffer::GBuffer()
{
    glGenFramebuffers(1, &m_fbo); 
    glGenTextures(GBUFFER_NUM_TEXTURES, m_textures);
    glGenTextures(1, &m_depthTexture);
}
GBuffer::~GBuffer()
{
    glDeleteFramebuffers(1, &m_fbo); 
    glDeleteTextures(GBUFFER_NUM_TEXTURES, m_textures);
    glDeleteTextures(1, &m_depthTexture);
}

bool GBuffer::Init(unsigned width, unsigned height)
{
	m_width = width;
	m_height = height;

    // Create the FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	// color textures
    for(unsigned i = 0 ; i < GBUFFER_NUM_TEXTURES; ++i) {
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }

    // depth texture
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 }; 
    glDrawBuffers(GBUFFER_NUM_TEXTURES, DrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "GBuffer error" << std::endl;
        return false;
    }
    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_width, m_height);
}

void GBuffer::BindForReading(GLenum textureUnit, unsigned type)
{
	if(type < GBUFFER_NUM_TEXTURES)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_textures[type]);
	}
}