#include "SceneBase.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>

map<string, Mesh*> SceneBase::meshList;
Camera3*SceneBase::camera;

SceneBase::SceneBase() : elapsedTime(0.f), fps(0), peakFPS(0), minFPS(INT_MAX)
{
}

SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_programID = LoadShaders( "Shader//Fog.vertexshader", "Shader//Fog.fragmentshader" );
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
	m_parameters[U_COLOR_SCALE_ENABLED] = glGetUniformLocation(m_programID, "colorScaleEnabled");
	m_parameters[U_COLOR_SCALE] = glGetUniformLocation(m_programID, "colorScale");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	m_parameters[U_FOG_COLOR] = glGetUniformLocation(m_programID, "fogParam.color");
	m_parameters[U_FOG_START] = glGetUniformLocation(m_programID, "fogParam.start");
	m_parameters[U_FOG_END] = glGetUniformLocation(m_programID, "fogParam.end");
	m_parameters[U_FOG_DENSITY] = glGetUniformLocation(m_programID, "fogParam.density");
	m_parameters[U_FOG_TYPE] = glGetUniformLocation(m_programID, "fogParam.type");
	m_parameters[U_FOG_ENABLED] = glGetUniformLocation(m_programID, "fogParam.enabled");

	glUseProgram(m_programID);

	glUniform1f(m_parameters[U_ALPHA], 1);
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glUniform1i(m_parameters[U_PARTICLE_TEXTURE], 0);

	Color fogColor(86.f/255.f,117.f/255.f,130.f/255.f);
	glUniform3fv(m_parameters[U_FOG_COLOR], 1, &fogColor.r);
	glUniform1f(m_parameters[U_FOG_START], 10);
	glUniform1f(m_parameters[U_FOG_END], 1000);
	glUniform1f(m_parameters[U_FOG_DENSITY], 0.025f);
	glUniform1i(m_parameters[U_FOG_TYPE], 2);
	glUniform1i(m_parameters[U_FOG_ENABLED], 1);
	
	InitMesh();
}

void SceneBase::InitParameters()
{
}

void SceneBase::InitMesh()
{
	cout << "Loading Mesh..." << endl;

	/* Basic Geometry */
	meshList["WIREBLOCK"] = MeshBuilder::GenerateCubeOutline("WireBlock", Color(1, 1, 1), 1.f, 1.f, 1.f);

	meshList["CUBE"] = MeshBuilder::GenerateCubeOnPlane("Cube", Color(1, 1, 1), 1, 1, 1, 1);

	meshList["CIRCLE"] = MeshBuilder::GenerateCircle("Cube", Color(1, 1, 1), 1);

	meshList["SPHERE"] = MeshBuilder::GenerateSphere("Sphere", Color(1, 1, 1), 16, 16, 1.f);

	meshList["CYLINDER"] = MeshBuilder::GenerateCylinder("Cylinder", Color(0.5f, 0.66f, 1.f), 72, 256, 1.f);

	meshList["QUAD"] = MeshBuilder::GenerateXYQuad("Quad", Color(1, 1, 1), 1, 1, 1);

	meshList["LINE"] = MeshBuilder::GenerateLine("Line", Color(1, 1, 1), 1.f);

	meshList["AXES"] = MeshBuilder::GenerateAxes("reference");

	meshList["BLOCK"] = MeshBuilder::GenerateBlock("Block", Color(1, 1, 1), 1, 1, 1);
	meshList["BLOCK"]->textureID = Load2DTGA("Assets//Block.tga", false);

	meshList["STAIR"] = MeshBuilder::GenerateStair("Block", Color(1, 1, 1), 1, 1, 1, 1);
	meshList["STAIR"]->textureID = meshList["BLOCK"]->textureID;

	meshList["MT_BLOCK"] = MeshBuilder::GenerateGrassBlock("Block", Color(1, 1, 1), 1, 1, 1, 1);
	meshList["MT_BLOCK"]->textureID = Load2DTGA("Assets//mtBlock.tga", false);

	meshList["HORSE"] = MeshBuilder::GenerateOBJ("Block", "Assets//Horse//horse.obj");
	meshList["HORSE"]->textureID = textureID["HORSE"] = Load2DTGA("Assets//Horse//npc_horse.tga");

	meshList["ARROW"] = MeshBuilder::GenerateOBJ("Arrow", "Assets//arrow.obj");
	meshList["ARROW"]->textureID = textureID["ARROW"] = Load2DTGA("Assets//Arrow.tga");

	meshList["BOW"] = MeshBuilder::GenerateOBJ("Bow", "Assets//Bow.obj");
	meshList["BOW"]->textureID = Load2DTGA("Assets//Bow.tga");

	meshList["MEAT"] = MeshBuilder::GenerateOBJ("Meat", "Assets//Meat.obj");
	meshList["MEAT"]->textureID = LoadTGA("Assets//Meat.tga");

	meshList["KNIFE"] = MeshBuilder::GenerateOBJ("Knife", "Assets//M9.obj");
	meshList["KNIFE"]->textureID = Load2DTGA("Assets//Knife.tga");

	meshList["HEAD"] = MeshBuilder::GenerateOBJ("Block", "Assets//Player//Head.obj");
	meshList["BODY"] = MeshBuilder::GenerateOBJ("Block", "Assets//Player//Body.obj");
	meshList["L_ARM"] = MeshBuilder::GenerateOBJ("Block", "Assets//Player//LeftArm.obj");
	meshList["L_LEG"] = MeshBuilder::GenerateOBJ("Block", "Assets//Player//LeftLeg.obj");
	meshList["R_ARM"] = MeshBuilder::GenerateOBJ("Block", "Assets//Player//RightArm.obj");
	meshList["R_LEG"] = MeshBuilder::GenerateOBJ("Block", "Assets//Player//RightLeg.obj");

	meshList["WOLF_HEAD"] = MeshBuilder::GenerateOBJ("Block", "Assets//Wolf//Head.obj");
	meshList["WOLF_BODY"] = MeshBuilder::GenerateOBJ("Block", "Assets//Wolf//Body.obj");
	meshList["WOLF_TAIL"] = MeshBuilder::GenerateOBJ("Block", "Assets//Wolf//Tail.obj");
	meshList["WOLF_LEG"] = MeshBuilder::GenerateOBJ("Block", "Assets//Wolf//Leg.obj");

	textureID["MAP"] = Load2DTGA("Assets//map.tga", false, false);
	textureID["MAP_LION"] = Load2DTGA("Assets//lion.tga", false, false);
	textureID["MAP_CASTLE"] = Load2DTGA("Assets//castle.tga", false, false);
	textureID["MAP_PLAYER"] = Load2DTGA("Assets//player.tga", false, false);
	textureID["SLOT"] = Load2DTGA("Assets//Slot.tga", false, false);
	textureID["SELECTOR"] = Load2DTGA("Assets//Selector.tga", false);
	textureID["SKYBOX"] = LoadTGA("Assets//Skybox.tga");
	textureID["TEXT"] = Load2DTGA("Assets//Font//font.tga");
	textureID["PLAYER1"] = Load2DTGA("Assets//Player//steve.tga");
	textureID["PLAYER2"] = Load2DTGA("Assets//Player//gregor.tga");
	textureID["PLAYER3"] = Load2DTGA("Assets//Player//sand.tga");
	textureID["PLAYER4"] = Load2DTGA("Assets//Player//silver.tga");
	textureID["WOLF"] = Load2DTGA("Assets//Wolf//wolf.tga");
	textureID["SPIRITBAR"] = Load2DTGA("Assets//SpiritBar.tga", false);

	cout << "Mesh Loaded." << endl << endl;
}

void SceneBase::UpdateInput(const unsigned char key)
{
}
void SceneBase::UpdateWeaponStatus(const unsigned char key)
{
}

void SceneBase::UpdateLight(unsigned count)
{
	for (unsigned i = 0; i < count; i++)
	{
		glUniform1i(m_parameters[U_LIGHT0_TYPE + i*11], lights[i].type);
		glUniform3fv(m_parameters[U_LIGHT0_COLOR + i*11], 1, &lights[i].color.r);
		glUniform1f(m_parameters[U_LIGHT0_POWER + i*11], lights[i].power);
		glUniform1f(m_parameters[U_LIGHT0_KC + i*11], lights[i].kC);
		glUniform1f(m_parameters[U_LIGHT0_KL + i*11], lights[i].kL);
		glUniform1f(m_parameters[U_LIGHT0_KQ + i*11], lights[i].kQ);
		glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF + i*11], lights[i].cosCutoff);
		glUniform1f(m_parameters[U_LIGHT0_COSINNER + i*11], lights[i].cosInner);
		glUniform1f(m_parameters[U_LIGHT0_EXPONENT + i*11], lights[i].exponent);
	}
}

void SceneBase::Update(double dt)
{
	if (Application::IsKeyPressed('Z'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (int(elapsedTime * 20) % 2)
	{
		fps = (float)(1.f / dt);

		if (minFPS > fps)
			minFPS = fps;
		else if (fps > peakFPS)
			peakFPS = fps;
	}

	SpriteAnimation *sa = dynamic_cast<SpriteAnimation*>(meshList["SPRITE"]);
	if(sa)
	{
		sa->Update(dt);
	}
	sa = dynamic_cast<SpriteAnimation*>(meshList["EXPLOSION"]);
	if(sa)
	{
		sa->Update(dt);
	} 
}

void SceneBase::RenderText(std::string text, Color color, Mesh* mesh)
{
	glUniform1i(m_parameters[U_TEXTURE_ROWS], 16);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID["TEXT"]);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	vector<Mtx44> MMat;
	vector<TexCoord> texOffset;
	float accumulatedSpace = (Application::textspace[text[0]] / 2);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing; 
		characterSpacing.SetToTranslation(accumulatedSpace, 0, 0);
		MMat.push_back(modelStack.Top() * characterSpacing);
		texOffset.push_back(TexCoord((text[i] % 16) / 16.f, 1 - ((text[i] / 16) + 1) / 16.f));

		if (text.length() > 1 && i <= text.length() - 2)
			accumulatedSpace += Application::textspace[text[i]]/2 + Application::textspace[text[i + 1]]/2;
	}

	mesh->Render(text.length(), &MMat[0], &texOffset[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glUniform1i(m_parameters[U_TEXTURE_ROWS], 0);
}
void SceneBase::RenderInstance(Mesh *mesh, unsigned NumInstance, const Mtx44* MMat, bool EnableLight, Color color)
{
	if (EnableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);

	if (color.r != 1 && color.g != 1 && color.b != 1)
	{
		glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
		glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	}

	if (mesh->textureID > 0)
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	else
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	mesh->Render(NumInstance, MMat); //this line should only be called once 
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

void SceneBase::RenderInstanceAtlas(Mesh * mesh, unsigned NumInstance, const Mtx44 * MMat, const TexCoord * offset, bool EnableLight, Color color)
{
	if (EnableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);

	if (color.r != 1 && color.g != 1 && color.b != 1)
	{
		glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
		glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	}

	if (mesh->textureID > 0)
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	else
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	mesh->Render(NumInstance, MMat, offset); //this line should only be called once 
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

void SceneBase::RenderMeshAtlas(Mesh *mesh, const TexCoord* offset, bool EnableLight, Color color)
{
	if (EnableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);

	if (color.r != 1 && color.g != 1 && color.b != 1)
	{
		glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
		glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	}

	if (mesh->textureID > 0)
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	else
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	mesh->Render(1, &modelStack.Top(), offset); //this line should only be called once 
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

void SceneBase::RenderMesh(Mesh *mesh, bool EnableLight, Color color)
{
	if (EnableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);

	if (color.r != 1 && color.g != 1 && color.b != 1)
	{
		glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
		glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	}

	if (mesh->textureID > 0)
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	else
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	mesh->Render(1, &modelStack.Top()); //this line should only be called once 
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

int SceneBase::getScreen_Width()
{
	return 0;
}
int SceneBase::getScreen_Height()
{
	return 0;
}

void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::RenderLight()
{
	for (int i = 0; i < 4; i++)
	{
		if(lights[i].type == Light::LIGHT_DIRECTIONAL)
		{
			Vector3 lightDir(lights[i].position.x, lights[i].position.y, lights[i].position.z);
			Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
			glUniform3fv(m_parameters[U_LIGHT0_POSITION + i*11], 1, &lightDirection_cameraspace.x);
		}
		else if(lights[i].type == Light::LIGHT_SPOT)
		{
			Position lightPosition_cameraspace = viewStack.Top() * lights[i].position;
			glUniform3fv(m_parameters[U_LIGHT0_POSITION + i*11], 1, &lightPosition_cameraspace.x);
			Vector3 spotDirection_cameraspace = viewStack.Top() * lights[i].spotDirection;
			glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION + i*11], 1, &spotDirection_cameraspace.x);
		}
		else
		{
			Position lightPosition_cameraspace = viewStack.Top() * lights[i].position;
			glUniform3fv(m_parameters[U_LIGHT0_POSITION + i*11], 1, &lightPosition_cameraspace.x);
		}
	}
}

void SceneBase::Exit()
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
}
