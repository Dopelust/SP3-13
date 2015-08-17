#ifndef SCENE_SHADOW_H
#define SCENE_SHADOW_H

#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "SceneBase.h"
#include "Camera.h"
#include "Camera3.h"
#include "Utility.h"
#include "MatrixStack.h"
#include "LoadTGA.h"
#include "Block.h"
#include "player.h"
#include "Particle.h"
#include "DepthFBO.h"

#include <sstream>
#include <queue>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace::std;

class SceneShadow : public SceneBase
{
public:
	enum RENDER_PASS
	{
		RENDER_PASS_PRE,
		RENDER_PASS_MAIN,
	};

	SceneShadow();
	virtual ~SceneShadow();

	void Init();
	void Exit();
	void RenderMesh(Mesh *mesh, bool EnableLight = true, Color color = Color(1, 1, 1));
	void RenderInstanceAtlas(Mesh *mesh, unsigned NumInstance, const Mtx44* MMat, const TexCoord* offset, bool EnableLight = true, Color color = Color(1, 1, 1));
	void RenderInstance(Mesh *mesh, unsigned NumInstance, const Mtx44* MMat, bool EnableLight = true, Color color = Color(1, 1, 1));

	void RenderPassGPass();
	void RenderPassMain();

protected:
	unsigned m_gPassShaderID;
	DepthFBO m_lightDepthFBO;
	Mtx44 m_lightDepthProj;
	Mtx44 m_lightDepthView;
	RENDER_PASS m_renderPass;
};

#endif