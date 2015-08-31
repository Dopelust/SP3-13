#ifndef SPRITE_ANIMATION_H
#define SPRITE_ANIMATION_H

#include "Mesh.h"
#include "Vector3.h"

struct Animation
{
	enum animationType
	{
		DEFAULT,
		LOOP,
		OSCILLATE,
	};

	Animation() {};
	~Animation() {};

	void Set(int startFrame, int endFrame, float time, animationType type=DEFAULT) 
	{
		this->default_startFrame = this->startFrame = startFrame; 
		this->default_endFrame = this->endFrame = endFrame; 
		this->type = type; 
		this->animTime = time;
		ended = false;
	}

	animationType type;

	int startFrame;
	int endFrame;
	int default_startFrame;
	int default_endFrame;

	float animTime;
	bool ended;
};

class SpriteAnimation : public Mesh
{
public:
	SpriteAnimation(const std::string &meshName, int row, int col, std::vector<Vertex> vertex_buffer_data, std::vector<GLuint> index_buffer_data);
	~SpriteAnimation();
	void Update(double dt);
	virtual void Render();
	void Render(unsigned NumInstance, const Mtx44* MMat);

	int m_row;
	int m_col;

	float m_currentTime;
	int m_currentFrame;
	int m_playCount;
	Animation *m_anim;

	void reset();
};

#endif