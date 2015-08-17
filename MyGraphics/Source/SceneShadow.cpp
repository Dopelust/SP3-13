#include "SceneShadow.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>

SceneShadow::SceneShadow()
{
}

SceneShadow::~SceneShadow()
{
}

void SceneShadow::Init()
{
	Application::setWindowSize(1184, 666);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_gPassShaderID = LoadShaders( "Shader//GPass.vertexshader", "Shader//GPass.fragmentshader" );
	m_parameters[U_LIGHT_DEPTH_VIEW_GPASS] = glGetUniformLocation(m_gPassShaderID, "lV");
	m_parameters[U_LIGHT_DEPTH_PROJECTION_GPASS] = glGetUniformLocation(m_gPassShaderID, "lP");

	m_programID = LoadShaders( "Shader//Shadow.vertexshader", "Shader//Shadow.fragmentshader" );
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters[U_PROJECTION] = glGetUniformLocation(m_programID, "P");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");

	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");
	
	m_parameters[U_LIGHT2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
	m_parameters[U_LIGHT2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
	m_parameters[U_LIGHT2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
	m_parameters[U_LIGHT2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
	m_parameters[U_LIGHT2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
	m_parameters[U_LIGHT2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");
	m_parameters[U_LIGHT2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
	m_parameters[U_LIGHT2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
	m_parameters[U_LIGHT2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
	m_parameters[U_LIGHT2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
	m_parameters[U_LIGHT2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");

	m_parameters[U_LIGHT3_POSITION] = glGetUniformLocation(m_programID, "lights[3].position_cameraspace");
	m_parameters[U_LIGHT3_COLOR] = glGetUniformLocation(m_programID, "lights[3].color");
	m_parameters[U_LIGHT3_POWER] = glGetUniformLocation(m_programID, "lights[3].power");
	m_parameters[U_LIGHT3_KC] = glGetUniformLocation(m_programID, "lights[3].kC");
	m_parameters[U_LIGHT3_KL] = glGetUniformLocation(m_programID, "lights[3].kL");
	m_parameters[U_LIGHT3_KQ] = glGetUniformLocation(m_programID, "lights[3].kQ");
	m_parameters[U_LIGHT3_TYPE] = glGetUniformLocation(m_programID, "lights[3].type");
	m_parameters[U_LIGHT3_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[3].spotDirection");
	m_parameters[U_LIGHT3_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[3].cosCutoff");
	m_parameters[U_LIGHT3_COSINNER] = glGetUniformLocation(m_programID, "lights[3].cosInner");
	m_parameters[U_LIGHT3_EXPONENT] = glGetUniformLocation(m_programID, "lights[3].exponent");

	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_ALPHA] = glGetUniformLocation(m_programID, "fragmentAlpha");
	m_parameters[U_TEXTURE_ROWS] = glGetUniformLocation(m_programID, "texRows");
	m_parameters[U_PARTICLE_TEXTURE] = glGetUniformLocation(m_programID, "particleTex");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	m_parameters[U_FOG_COLOR] = glGetUniformLocation(m_programID, "fogParam.color");
	m_parameters[U_FOG_START] = glGetUniformLocation(m_programID, "fogParam.start");
	m_parameters[U_FOG_END] = glGetUniformLocation(m_programID, "fogParam.end");
	m_parameters[U_FOG_DENSITY] = glGetUniformLocation(m_programID, "fogParam.density");
	m_parameters[U_FOG_TYPE] = glGetUniformLocation(m_programID, "fogParam.type");
	m_parameters[U_FOG_ENABLED] = glGetUniformLocation(m_programID, "fogParam.enabled");

	m_parameters[U_LIGHT_DEPTH_VIEW] = glGetUniformLocation(m_programID, "lightView");
	m_parameters[U_LIGHT_DEPTH_PROJECTION] = glGetUniformLocation(m_programID, "lightProj");
	m_parameters[U_SHADOW_MAP] = glGetUniformLocation(m_programID, "shadowMap");
		
	glUseProgram(m_programID);	

	glUniform1f(m_parameters[U_ALPHA], 1);
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glUniform1i(m_parameters[U_PARTICLE_TEXTURE], 0);

	Color fogColor(86.f / 255.f, 117.f / 255.f, 130.f / 255.f);
	glUniform3fv(m_parameters[U_FOG_COLOR], 1, &fogColor.r);
	glUniform1f(m_parameters[U_FOG_START], 10);
	glUniform1f(m_parameters[U_FOG_END], 1000);
	glUniform1f(m_parameters[U_FOG_DENSITY], 0.025f);
	glUniform1i(m_parameters[U_FOG_TYPE], 2);
	glUniform1i(m_parameters[U_FOG_ENABLED], 1);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 5, 5);
	lights[1].power = 0.f;
	lights[2].power = 0.f;
	lights[3].power = 0.f;

	UpdateLight();

	m_lightDepthFBO.Init(2048, 2048);
	InitMesh();
}

void SceneShadow::RenderMesh(Mesh *mesh, bool EnableLight, Color color)
{
	if (m_renderPass == RENDER_PASS_PRE)
	{
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_VIEW_GPASS], 1, GL_FALSE, &m_lightDepthView.a[0]);
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_PROJECTION_GPASS], 1, GL_FALSE, &m_lightDepthProj.a[0]);
		mesh->Render(1, &modelStack.Top()); //this line should only be called once 
	}
	else
	{
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_VIEW], 1, GL_FALSE, &m_lightDepthView.a[0]);
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_PROJECTION], 1, GL_FALSE, &m_lightDepthProj.a[0]);
		SceneBase::RenderMesh(mesh, EnableLight, color);
	}
}

void SceneShadow::RenderInstanceAtlas(Mesh * mesh, unsigned NumInstance, const Mtx44 * MMat, const TexCoord * offset, bool EnableLight, Color color)
{
	if (m_renderPass == RENDER_PASS_PRE)
	{
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_VIEW_GPASS], 1, GL_FALSE, &m_lightDepthView.a[0]);
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_PROJECTION_GPASS], 1, GL_FALSE, &m_lightDepthProj.a[0]);
		mesh->Render(NumInstance, MMat, offset); //this line should only be called once 
	}
	else
	{
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_VIEW], 1, GL_FALSE, &m_lightDepthView.a[0]);
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_PROJECTION], 1, GL_FALSE, &m_lightDepthProj.a[0]);
		SceneBase::RenderInstanceAtlas(mesh, NumInstance, MMat, offset, EnableLight, color);
	}
}

void SceneShadow::RenderInstance(Mesh * mesh, unsigned NumInstance, const Mtx44 * MMat, bool EnableLight, Color color)
{
	if (m_renderPass == RENDER_PASS_PRE)
	{
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_VIEW_GPASS], 1, GL_FALSE, &m_lightDepthView.a[0]);
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_PROJECTION_GPASS], 1, GL_FALSE, &m_lightDepthProj.a[0]);
		mesh->Render(NumInstance, MMat); //this line should only be called once 
	}
	else
	{
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_VIEW], 1, GL_FALSE, &m_lightDepthView.a[0]);
		glUniformMatrix4fv(m_parameters[U_LIGHT_DEPTH_PROJECTION], 1, GL_FALSE, &m_lightDepthProj.a[0]);
		SceneBase::RenderInstance(mesh, NumInstance, MMat, EnableLight, color);
	}
}

void SceneShadow::RenderPassGPass()
{
	m_renderPass = RENDER_PASS_PRE;
	
	m_lightDepthFBO.BindForWriting();	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_gPassShaderID);

	if(lights[0].type == Light::LIGHT_DIRECTIONAL)
		m_lightDepthProj.SetToOrtho(-48, 48, -48, 48, -16, 16);
	else
		m_lightDepthProj.SetToPerspective(90, 1.f, 0.1, 20);
	m_lightDepthView.SetToLookAt((int)lights[0].position.x, 0, (int)lights[0].position.z, (int)lights[0].position.x, -1, (int)lights[0].position.z, 0, 0, 1);
}

void SceneShadow::RenderPassMain()
{
	m_renderPass = RENDER_PASS_MAIN;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Application::m_width, Application::m_height);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_programID);
	m_lightDepthFBO.BindForReading(GL_TEXTURE8);
	glUniform1i(m_parameters[U_SHADOW_MAP], 8);
}

void SceneShadow::Exit()
{
	// Cleanup VBO
	for(map<string, Mesh*>::iterator it = meshList.begin(); it != meshList.end(); ++it)
	{
		if(it->second)
		{
			delete it->second;
		}
	}
	meshList.clear();

	glDeleteProgram(m_programID);
	glDeleteProgram(m_gPassShaderID);
}
