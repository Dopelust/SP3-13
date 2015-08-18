#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "Minimap.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Camera.h"
#include "Camera3.h"
#include "Utility.h"
#include "MatrixStack.h"
#include "LoadTGA.h"
#include "Block.h"
#include "player.h"
#include "Particle.h"

#include <sstream>
#include <queue>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace::std;

class SceneBase : public Scene
{
public:
	enum UNIFORM_TYPE
	{
		//MVP
		U_MVP, U_MODELVIEW, U_MODELVIEW_INVERSE_TRANSPOSE,
		U_VIEW, U_PROJECTION,
		//Material
		U_MATERIAL_AMBIENT, U_MATERIAL_DIFFUSE,	U_MATERIAL_SPECULAR, U_MATERIAL_SHININESS,
		//Light
		U_LIGHTENABLED,
		U_LIGHT0_POSITION, U_LIGHT0_COLOR, U_LIGHT0_POWER, U_LIGHT0_KC, U_LIGHT0_KL, U_LIGHT0_KQ, U_LIGHT0_TYPE, U_LIGHT0_SPOTDIRECTION, U_LIGHT0_COSCUTOFF, U_LIGHT0_COSINNER, U_LIGHT0_EXPONENT,
		U_LIGHT1_POSITION, U_LIGHT1_COLOR, U_LIGHT1_POWER, U_LIGHT1_KC, U_LIGHT1_KL, U_LIGHT1_KQ, U_LIGHT1_TYPE, U_LIGHT1_SPOTDIRECTION, U_LIGHT1_COSCUTOFF, U_LIGHT1_COSINNER, U_LIGHT1_EXPONENT,
		U_LIGHT2_POSITION, U_LIGHT2_COLOR, U_LIGHT2_POWER, U_LIGHT2_KC, U_LIGHT2_KL, U_LIGHT2_KQ, U_LIGHT2_TYPE, U_LIGHT2_SPOTDIRECTION, U_LIGHT2_COSCUTOFF, U_LIGHT2_COSINNER, U_LIGHT2_EXPONENT,
		U_LIGHT3_POSITION, U_LIGHT3_COLOR, U_LIGHT3_POWER, U_LIGHT3_KC, U_LIGHT3_KL, U_LIGHT3_KQ, U_LIGHT3_TYPE, U_LIGHT3_SPOTDIRECTION, U_LIGHT3_COSCUTOFF, U_LIGHT3_COSINNER, U_LIGHT3_EXPONENT,
		U_NUMLIGHTS,
		//Texture
		U_PARTICLE_TEXTURE, U_ALPHA, U_TEXTURE_ROWS,
		U_COLOR_TEXTURE_ENABLED, U_COLOR_TEXTURE,
		U_COLOR_SCALE_ENABLED, U_COLOR_SCALE,
		U_TEXT_ENABLED, U_TEXT_COLOR,
		//Fog
		U_FOG_COLOR, U_FOG_START, U_FOG_END, U_FOG_DENSITY, U_FOG_TYPE, U_FOG_ENABLED,
		//Shadow
		U_LIGHT_DEPTH_VIEW_GPASS, U_LIGHT_DEPTH_PROJECTION_GPASS, U_LIGHT_DEPTH_VIEW, U_LIGHT_DEPTH_PROJECTION, U_SHADOW_MAP,

		U_MVP_GBUFFER,
		U_MODELVIEW_GBUFFER,
		U_MODELVIEW_INVERSE_TRANSPOSE_GBUFFER,
		U_MATERIAL_AMBIENT_GBUFFER,
		U_MATERIAL_DIFFUSE_GBUFFER,
		U_MATERIAL_SPECULAR_GBUFFER,
		U_MATERIAL_EMISSIVE_GBUFFER,
		U_MATERIAL_SHININESS_GBUFFER,
		U_LIGHTENABLED_GBUFFER,
		U_COLOR_TEXTURE_ENABLED_GBUFFER,
		U_COLOR_TEXTURE_ENABLED1_GBUFFER,
		U_COLOR_TEXTURE_GBUFFER,
		U_COLOR_TEXTURE1_GBUFFER,
		U_LIGHT_DEPTH_MVP_GBUFFER,
		U_SHADOW_MAP_GBUFFER,

		U_MVP_LIGHTPASS,
		U_SCREEN_SIZE_LIGHTPASS,
		U_POSITION_MAP_LIGHTPASS,
		U_NORMAL_MAP_LIGHTPASS,
		U_AMBIENT_MAP_LIGHTPASS,
		U_DIFFUSE_MAP_LIGHTPASS,
		U_SPECULAR_MAP_LIGHTPASS,
		U_EMISSIVE_MAP_LIGHTPASS,
		U_LIGHT_TYPE_LIGHTPASS,
		U_LIGHT_POSITION_LIGHTPASS,
		U_LIGHT_COLOR_LIGHTPASS,
		U_LIGHT_POWER_LIGHTPASS,
		U_LIGHT_RADIUS_LIGHTPASS,

		U_TOTAL,
	};
	enum WEAPON_ACTION
	{
		WA_SWITCH1 = 0,
		WA_SWITCH2,
		WA_SWITCH,
		WA_FIRE,
		WA_RELOAD,
		WA_TOTAL,
	};

	static map<string, Mesh*> meshList;
	map<string, unsigned> textureID;

	SceneBase();
	virtual ~SceneBase();

	virtual void Init();
	virtual void InitParameters();
	void InitMesh();
	virtual void UpdateLight(unsigned count=4);
	virtual void Update(double dt);
	virtual void Render();
	virtual void RenderLight();
	virtual void Exit();
	virtual void UpdateInput (const unsigned char key);
	virtual void UpdateWeaponStatus (const unsigned char key);
	virtual int getScreen_Width();
	virtual int getScreen_Height();
	void RenderText(std::string text, Color color, Mesh* mesh=meshList["QUAD"]);
	virtual void RenderMesh(Mesh *mesh, bool EnableLight = true, Color color = Color(1, 1, 1));
	void RenderMeshAtlas(Mesh *mesh, const TexCoord* offset, bool EnableLight = true, Color color = Color(1, 1, 1));
	virtual void RenderInstance(Mesh *mesh, unsigned NumInstance, const Mtx44* MMat, bool EnableLight = true, Color color = Color(1, 1, 1));
	virtual void RenderInstanceAtlas(Mesh *mesh, unsigned NumInstance, const Mtx44* MMat, const TexCoord* offset, bool EnableLight = true, Color color = Color(1, 1, 1));

	static Camera3* camera;
protected:
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	Light lights[4];

	float fps, peakFPS, minFPS;
	float elapsedTime;
};

#endif