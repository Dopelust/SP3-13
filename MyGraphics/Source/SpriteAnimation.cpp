#include "SpriteAnimation.h"
#include "MyMath.h"
#include "vertex.h"
#include "GL\glew.h"

using namespace::std;

SpriteAnimation::SpriteAnimation(const std::string &meshName, int row, int col, std::vector<Vertex> vertex_buffer_data, std::vector<GLuint> index_buffer_data)
	: Mesh(meshName, vertex_buffer_data, index_buffer_data)
	, m_row(row)
	, m_col(col)
	, m_currentTime(0)
	, m_currentFrame(0)
	, m_playCount(0)
{
	m_anim = NULL;
} 

SpriteAnimation::~SpriteAnimation()
{
	if(m_anim)
		delete m_anim;
} 

void SpriteAnimation::Update(double dt)
{
	if(m_anim) // If animation happens
	{
		m_currentTime += dt;

		//int numFrame= Math::Max(1,(m_anim->endFrame - m_anim->startFrame + 1));
		int numFrame= Math::Max(1, m_anim->default_endFrame + 1);
		float frameTime = m_anim->animTime / numFrame;
	
		if (m_anim->startFrame > m_anim->endFrame)
			m_currentFrame = Math::Min(m_anim->default_endFrame,( m_anim->startFrame - static_cast<int>(m_currentTime / frameTime)));
		else
			m_currentFrame = Math::Min(m_anim->default_endFrame,( m_anim->startFrame + static_cast<int>(m_currentTime / frameTime)));

		if(m_currentTime > m_anim->animTime)
		{
			if(m_anim->type == Animation::LOOP)
			{
				m_playCount++;
				m_currentTime = 0;
				m_currentFrame = m_anim->startFrame;
			}
			else if(m_anim->type == Animation::OSCILLATE)
			{
				m_currentTime = frameTime;
				if (m_anim->startFrame == m_anim->default_startFrame)
				{
					m_anim->startFrame = m_anim->default_endFrame;
					m_anim->endFrame = m_anim->default_startFrame;
				}
				else
				{
					m_anim->startFrame = m_anim->default_startFrame;
					m_anim->endFrame = m_anim->default_endFrame;
				}
				m_currentFrame = m_anim->startFrame;
			}
			else
				m_anim->ended = true;
		}
	}
}

void SpriteAnimation::Render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, (void*)(m_currentFrame * 6 * sizeof(GLuint)));
} 

void SpriteAnimation::Render(unsigned NumInstance, const Mtx44* MMat)
{
	glBindBuffer(GL_ARRAY_BUFFER, MBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Mtx44) * NumInstance, MMat, GL_DYNAMIC_DRAW);

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, (void*)(m_currentFrame * 6 * sizeof(GLuint)), NumInstance);
}
void SpriteAnimation::reset()
{
	m_anim->startFrame = m_anim->default_startFrame;
	m_anim->endFrame = m_anim->default_endFrame;

	m_anim->ended = false;
	m_currentTime = 0;
	m_currentFrame = m_anim->startFrame;

}