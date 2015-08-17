/******************************************************************************/
/*!
\file	MinScene.cpp
\author Ricsson
\par	
\brief
This is the cpp
*/
/******************************************************************************/
#include "MinScene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "Mtx44.h"

#include "Application.h"
#include "MeshBuilder.h"

#include "GLFW\glfw3.h"

#include <istream>
#include <algorithm>
#include <iomanip>

using namespace::std;

extern ISoundEngine * engine;

MinScene::MinScene() :selectedBlock(NULL), shouldRenderChat(false), chatInputLimiter(0.f), nextUpdate(0)
{
}

MinScene::~MinScene()
{
	
}

void MinScene::Init()
{
	Application::setWindowSize(1184, 666);
	SceneShadow::Init();
	glClearColor(0.5f, 0.66f, 1.f, 0.0f);

	cout << "Initializing Light..." << endl;

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0,10,5);
	lights[0].power = 1.f;

	UpdateLight(1);

	cout << "Light Initialized." << endl << endl;

	player.Init();
	player.position.y = 3;
	camera = &player.camera;

	selectedBlock = NULL;
	shouldRenderChat = false;
	blockInventory.Init();
	RenderDist = 80;

	worldBlocks = 0;
	for (unsigned z = 0; z < worldZ; ++z)
	{
		for (unsigned y = 0; y < worldY; ++y)
		{
			for (unsigned x = 0; x < worldX; ++x)
			{
				worldBlockList[x][y][z] = NULL;
			}
		}
	}
	cout << "Initializing Objects..." << endl;

	//GenerateWorld(Vector3(worldX, 1, worldZ));
	Load("Save//save1.txt");

	cout << worldBlocks <<  " Objects Initialized." << endl << endl;

	camera->position = player.position; camera->position.y += player.eyeLevel;
	soundInit();

	for (unsigned i = 0; i < 20; ++i)
	{
		Living* entity = new Living();
		entity->position.Set(rand()%101 - 50, 1, rand() % 101 - 5);
		entity->hOrientation = rand() % 360;
		entity->collision.hitbox.Set(0.6f, 1.8f, 0.6f);
		entity->collision.centre.Set(0, 0.9f, 0);
		entity->id = rand() % 3;
		worldLivingThings.push_back(entity);
	}
}

bool MinScene::FetchBlock(Vector3 pos, bool checkForCollision, unsigned blockID, Block::blockType type)
{
	if (pos.x >= worldX || pos.x < 0 || pos.y >= worldY || pos.y < 0 || pos.y >= worldY || pos.x < 0)
		return false;
	if (worldBlockList[(int)pos.x][(int)pos.y][(int)pos.z])
		return false;

	Block * o = new Block();
	o->position.Set(pos.x - worldX*0.5f, pos.y - worldY * 0.5f, pos.z - worldZ * 0.5f);
	o->collision.hitbox.Set(1, 1, 1);
	o->collision.centre.Set(0.5f, 0.5f, 0.5f);
	o->id = blockID;
	
	if (blockID < 4)
		o->type = Block::MT;
	else
		o->type = type;

	if (checkForCollision)
	{
		Block Player(player.position, player.collision.centre, player.collision.hitbox);
		if (Block::checkCollision(*o, Player))
		{
			delete o;
			return false;
		}

		unsigned count = LivingThings.size();
		for (unsigned i = 0; i < count; ++i)
		{
			if (Block::checkCollision(*o, Block(LivingThings[i]->position, LivingThings[i]->collision.centre, LivingThings[i]->collision.hitbox)))
			{
				delete o;
				return false;
			}
		}
	}

	if (type == Block::TRANS)
		alphaBlockList.push_back(o);
	blockList.push_back(o);
	worldBlockList[(int)pos.x][(int)pos.y][(int)pos.z] = o;
	worldBlocks++;

	return true;
}

bool MinScene::FetchStair(Vector3 pos, bool checkForCollision, unsigned blockID, int orientation, int look)
{
	if (pos.x >= worldX || pos.x < 0 || pos.y >= worldY || pos.y < 0 || pos.y >= worldY || pos.x < 0)
		return false;
	if (worldBlockList[(int)pos.x][(int)pos.y][(int)pos.z])
		return false;

	Stair * o = new Stair();
	o->position.Set(pos.x - worldX*0.5f, pos.y - worldY * 0.5f, pos.z - worldZ * 0.5f);
	o->collision.hitbox.Set(1, 1, 1);
	o->collision.centre.Set(0.5f, 0.5f, 0.5f);
	o->id = blockID;
	o->type = Block::STAIR;
	o->orientation = orientation;
	o->look = look;

	Collision c;
	switch (orientation)
	{
	case 0:
		c.hitbox.Set(1, 0.5f, 0.5f);
		if (look == 90)
			c.centre.Set(0.5f, 0.75f, 0.75f);
		else
			c.centre.Set(0.5f, 0.25f, 0.75f);
		o->collisions.push_back(c);
		c.hitbox.Set(1, 1, 0.5f);
		c.centre.Set(0.5f, 0.5f, 0.25f);
		o->collisions.push_back(c);
		break;
	case 180:
		c.hitbox.Set(1, 0.5f, 0.5f);
		if (look == 90)
			c.centre.Set(0.5f, 0.75f, 0.25f);
		else
			c.centre.Set(0.5f, 0.25f, 0.25f);
		o->collisions.push_back(c);
		c.hitbox.Set(1, 1, 0.5f);
		c.centre.Set(0.5f, 0.5f, 0.75f);
		o->collisions.push_back(c);
		break;
	case 90:
		c.hitbox.Set(0.5f, 0.5f, 1);
		if (look == 90)
			c.centre.Set(0.75f, 0.75f, 0.5f);
		else
			c.centre.Set(0.75f, 0.25f, 0.5f);
		o->collisions.push_back(c);
		c.hitbox.Set(0.5f, 1, 1);
		c.centre.Set(0.25f, 0.5f, 0.5f);
		o->collisions.push_back(c);
		break;
	case -90:
		c.hitbox.Set(0.5f, 0.5f, 1);
		if (look == 90)
			c.centre.Set(0.25f, 0.75f, 0.5f);
		else
			c.centre.Set(0.25f, 0.25f, 0.5f);
		o->collisions.push_back(c);
		c.hitbox.Set(0.5f, 1, 1);
		c.centre.Set(0.75f, 0.5f, 0.5f);
		o->collisions.push_back(c);
		break;
	}

	if (checkForCollision)
	{
		Block Player(player.position, player.collision.centre, player.collision.hitbox);
		Block Blk1(o->position, o->collisions[0].centre, o->collisions[0].hitbox);
		Block Blk2(o->position, o->collisions[1].centre, o->collisions[1].hitbox);
		if (Block::checkCollision(Blk1, Player) || Block::checkCollision(Blk2, Player))
		{
			delete o;
			return false;
		}

		unsigned count = LivingThings.size();
		for (unsigned i = 0; i < count; ++i)
		{
			if (Block::checkCollision(Blk1, Block(LivingThings[i]->position, LivingThings[i]->collision.centre, LivingThings[i]->collision.hitbox)) ||
				Block::checkCollision(Blk2, Block(LivingThings[i]->position, LivingThings[i]->collision.centre, LivingThings[i]->collision.hitbox)))
			{
				delete o;
				return false;
			}
		}
	}

	blockList.push_back(o);
	worldBlockList[(int)pos.x][(int)pos.y][(int)pos.z] = o;
	worldBlocks++;
	return true;
}

bool MinScene::SpawnParticle(Vector3 pos, unsigned id)
{
	Particle p;
	p.position = pos;
	p.velocity.Set(Math::RandFloatMinMax(-1.75f, 1.75f), rand() % 4 + 2, Math::RandFloatMinMax(-1.75f, 1.75f));
	p.size.Set(Math::RandFloatMinMax(0.05f, 0.15f), Math::RandFloatMinMax(0.05f, 0.15f), Math::RandFloatMinMax(0.05f, 0.15f));
	p.blockID = id;

	particleList.push_back(p);
	return true;
}

void MinScene::UpdateInput(const unsigned char key)
{
	if (!shouldRenderChat)
	{
		for (unsigned i = 0; i < 255; ++i)
		{
			if (key == i)
				player.myKeys[i] = true;
		}
	}
}

Vector3 MinScene::PositionToIndex(Vector3 pos)
{
	return Vector3(pos.x + worldX*0.5f, pos.y + worldY*0.5f, pos.z + worldZ*0.5f);
}

void MinScene::ObtainBlockList()
{	
	unsigned count = LivingThings.size();
	for (unsigned i = 0; i < count; ++i)
		LivingThings[i]->collisionBlockList.clear();
	player.collisionBlockList.clear();

	LivingThings.clear();
	alphaBlockList.clear();
	blockList.clear();

	count = worldLivingThings.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (camera->position.DistSquared(worldLivingThings[i]->position) < RenderDist * RenderDist)
			LivingThings.push_back(worldLivingThings[i]);
	}
	count = LivingThings.size();

	Vector3 index = PositionToIndex(camera->position);
	int zBegin = Math::Max(0, (int)(index.z - RenderDist));
	int zEnd = Math::Min(worldZ, (int)(index.z + RenderDist)); zEnd = zEnd < 0 ? 0 : zEnd;
	int yBegin = Math::Max(0, (int)(index.y - RenderDist));
	int yEnd = Math::Min(worldY, (int)(index.y + RenderDist)); yEnd = yEnd < 0 ? 0 : yEnd;
	int xBegin = Math::Max(0, (int)(index.x - RenderDist));
	int xEnd = Math::Min(worldX, (int)(index.x + RenderDist)); xEnd = xEnd < 0 ? 0 : xEnd;

	Vector3 view = (camera->target - camera->position).Normalized();

	float pZ = view.Dot(Vector3(0, 0, 1));
	float nZ = view.Dot(Vector3(0, 0, -1));
	float pX = view.Dot(Vector3(1, 0, 0));
	float nX = view.Dot(Vector3(-1, 0, 0));

	if (nZ > pZ && nZ > pX && nZ > nX) //South
	{
		for (int z = zEnd - 1; z >= zBegin; --z)
		{
			for (unsigned y = yBegin; y < yEnd; ++y)
			{
				for (unsigned x = xBegin; x < xEnd; ++x)
				{
					if (worldBlockList[x][y][z])
					{
						blockList.push_back(worldBlockList[x][y][z]);

						if (worldBlockList[x][y][z]->type == Block::TRANS)
							alphaBlockList.push_back(worldBlockList[x][y][z]);

						for (unsigned i = 0; i < count; ++i)
						{
							if (LivingThings[i]->position.DistSquared(worldBlockList[x][y][z]->position) < 16 * 16)
								LivingThings[i]->collisionBlockList.push_back(worldBlockList[x][y][z]);
						}

						if (player.position.DistSquared(worldBlockList[x][y][z]->position) < 16 * 16)
							player.collisionBlockList.push_back(worldBlockList[x][y][z]);
					}
				}
			}
		}
	}
	else if (nX > pZ && nX > nZ && nX > pX) //East
	{
		for (int z = zEnd - 1; z >= zBegin; --z)
		{
			for (unsigned y = yBegin; y < yEnd; ++y)
			{
				for (int x = xEnd - 1; x >= xBegin; --x)
				{
					if (worldBlockList[x][y][z])
					{
						blockList.push_back(worldBlockList[x][y][z]);

						if (worldBlockList[x][y][z]->type == Block::TRANS)
							alphaBlockList.push_back(worldBlockList[x][y][z]);

						for (unsigned i = 0; i < count; ++i)
						{
							if (LivingThings[i]->position.DistSquared(worldBlockList[x][y][z]->position) < 16 * 16)
								LivingThings[i]->collisionBlockList.push_back(worldBlockList[x][y][z]);
						}

						if (player.position.DistSquared(worldBlockList[x][y][z]->position) < 16 * 16)
							player.collisionBlockList.push_back(worldBlockList[x][y][z]);
					}
				}
			}
		}
	}
	else //North, West
	{
		for (unsigned z = zBegin; z < zEnd; ++z)
		{
			for (unsigned y = yBegin; y < yEnd; ++y)
			{
				for (unsigned x = xBegin; x < xEnd; ++x)
				{
					if (worldBlockList[x][y][z])
					{
						blockList.push_back(worldBlockList[x][y][z]);

						if (worldBlockList[x][y][z]->type == Block::TRANS)
							alphaBlockList.push_back(worldBlockList[x][y][z]);

						for (unsigned i = 0; i < count; ++i)
						{
							if (LivingThings[i]->position.DistSquared(worldBlockList[x][y][z]->position) < 16 * 16)
								LivingThings[i]->collisionBlockList.push_back(worldBlockList[x][y][z]);
						}

						if (player.position.DistSquared(worldBlockList[x][y][z]->position) < 16 * 16)
							player.collisionBlockList.push_back(worldBlockList[x][y][z]);
					}
				}
			}
		}
	}

	sort(alphaBlockList.begin(), alphaBlockList.end(), IsMoreThan);
}

void MinScene::Update(double dt)
{
	SceneBase::Update(dt);
	blockInventory.Update();

	if (elapsedTime >= (nextUpdate*0.5f))
	{
		ObtainBlockList();
		nextUpdate++;
	}

	static bool bNButtonPressed = false;

	if (!bNButtonPressed && Application::IsKeyPressed('N'))
	{
		player.noClip = player.noClip ? false : true;
		bNButtonPressed = true;
	}
	else if (bNButtonPressed && !Application::IsKeyPressed('N'))
		bNButtonPressed = false;
		
	player.Update(dt, blockList, shouldRenderChat);
	soundUpdate(player);

	unsigned count = LivingThings.size();
	for (unsigned i = 0; i < count; ++i)
		LivingThings[i]->Update(dt, false);

	lights[0].position.Set(camera->position.x, 4, camera->position.z);

	if (!shouldRenderChat)
	{
		unsigned count = blockList.size();

		static bool bLMouseButtonPressed = false;

		if (!bLMouseButtonPressed && Application::IsMousePressed(0))
		{
			bLMouseButtonPressed = true;

			if (selectedBlock)
			{
				if (RemoveBlock(selectedBlock))
				{
					ISound * sound = engine->play2D(Block::getSound(selectedBlock->id), false, true);
					if (sound)
					{
						sound->setVolume(0.4f);
						sound->setIsPaused(false);
					}
				}
			}
		}
		else if ((bLMouseButtonPressed && !Application::IsMousePressed(0)))
		{
			bLMouseButtonPressed = false;
		}

		selectedBlock = NULL;

		float dist = INT_MAX;
		Vector3 view = (camera->target - camera->position).Normalized();

		count = blockList.size();
		for (unsigned i = 0; i < count; ++i)
		{
			float rayDist = blockList[i]->collisionWithRay(camera->position, view);
			if (rayDist >= 0 && rayDist <= 5)
			{
				if (rayDist < dist)
				{
					selectedBlock = blockList[i];
					dist = rayDist;
				}
			}
		}

		static bool bRMouseButtonPressed = false;
		static float bRMouseButtonElapsed = 0.f;

		if (!bRMouseButtonPressed && Application::IsMousePressed(1))
		{
			bRMouseButtonElapsed = 0.f;
			bRMouseButtonPressed = true;

			if (selectedBlock)
			{
				bool playSound = false;

				Vector3 newBlockPosition = PositionToIndex(selectedBlock->position);
				int face = selectedBlock->RayCollisionFace(camera->position, view);
				switch (face)
				{
				case 1: newBlockPosition.x--; break;
				case 2: newBlockPosition.x++; break;
				case 3: newBlockPosition.y--; break;
				case 4: newBlockPosition.y++; break;
				case 5: newBlockPosition.z--; break;
				case 6: newBlockPosition.z++; break;
				}

				if (blockInventory.getBlock().type == Block::STAIR)
				{
					Vector3 view = (camera->target - camera->position).Normalized();

					float pZ = view.Dot(Vector3(0, 0, 1));
					float nZ = view.Dot(Vector3(0, 0, -1));
					float pX = view.Dot(Vector3(1, 0, 0));
					float nX = view.Dot(Vector3(-1, 0, 0));

					int look = 0;
					if (face == 3)
						look = 90;
					else if (face != 4)
					{
						Block UpperBlock(selectedBlock->position, selectedBlock->collision.centre, selectedBlock->collision.hitbox);
						UpperBlock.position.y+=0.5f;
						Block LowerBlock(selectedBlock->position, selectedBlock->collision.centre, selectedBlock->collision.hitbox);
						LowerBlock.position.y-=0.5f;

						float up = UpperBlock.collisionWithRay(camera->position, view);
						float down = LowerBlock.collisionWithRay(camera->position, view);
						if (down == -1)
							look = 90;
						else if (up == -1)
							look = 0;
						else if (up < down)
							look = 90;
					}

					if (pZ > nZ && pZ > pX && pZ > nX)
						playSound = FetchStair(newBlockPosition, true, blockInventory.getBlock().id, 180, look);
					else if (pX > pZ && pX > nZ && pX > nX)
						playSound = FetchStair(newBlockPosition, true, blockInventory.getBlock().id, -90, look);
					else if (nZ > pZ && nZ > pX && nZ > nX)
						playSound = FetchStair(newBlockPosition, true, blockInventory.getBlock().id, 0, look);
					else
						playSound = FetchStair(newBlockPosition, true, blockInventory.getBlock().id, 90, look);
				}
				else
					playSound = FetchBlock(newBlockPosition, true, blockInventory.getBlock().id, blockInventory.getBlock().type);

				if (playSound)
				{
					ISound * sound = engine->play2D(Block::getSound(blockInventory.getBlock().id), false, true);
					if (sound)
					{
						sound->setVolume(0.4f);
						sound->setIsPaused(false);
					}
				}
			}
		}
		else if (bRMouseButtonElapsed >= 0.2f || (bRMouseButtonPressed && !Application::IsMousePressed(1)))
		{
			bRMouseButtonPressed = false;
		}

		if (bRMouseButtonPressed)
			bRMouseButtonElapsed += dt;

		static bool bMMouseButtonPressed = false;

		if (!bMMouseButtonPressed && Application::IsMousePressed(2))
		{
			bMMouseButtonPressed = true;

			if (selectedBlock)
				blockInventory.setBlock(*selectedBlock);
		}
		else if (bMMouseButtonPressed && !Application::IsMousePressed(2))
		{
			bMMouseButtonPressed = false;
		}
	}
	else
	{
		if (chatInputLimiter > 0.1f)
			writeLine();
		chatInputLimiter += dt;
	}

	count = particleList.size();
	for (int i = 0; i < count; ++i)
	{
		particleList[i].Update(dt);

		if (!particleList[i].IsActive())
		{
			particleList.erase(particleList.begin() + i);
			--i;
			--count;
		}
	}

	static bool bEButtonPressed = false;

	if (!bEButtonPressed && Application::IsKeyPressed('E'))
	{
		Save("Save//save1.txt");
		bEButtonPressed = true;
	}
	else if (bNButtonPressed && !Application::IsKeyPressed('E'))
		bEButtonPressed = false;
	
	elapsedTime += dt;
}
bool MinScene::RemoveBlock(Block* block)
{
	for (unsigned i = 0; i < 30; ++i)
		SpawnParticle(selectedBlock->position + selectedBlock->collision.centre, selectedBlock->id);

	Vector3 index = PositionToIndex(block->position);
	worldBlockList[(int)index.x][(int)index.y][(int)index.z] = NULL;

	unsigned count = blockList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (blockList[i] == selectedBlock)
		{
			blockList.erase(blockList.begin() + i);
			break;
		}
	}
	if (block->type == Block::TRANS)
	{
		count = alphaBlockList.size();
		for (unsigned i = 0; i < count; ++i)
		{
			if (alphaBlockList[i] == selectedBlock)
			{
				alphaBlockList.erase(alphaBlockList.begin() + i);
				break;
			}
		}
	}
	delete block;
	worldBlocks--;

	return true;
}
void MinScene::Render()
{
	RenderPassGPass();
	RenderBlocks_GPass();
	RenderEntities_GPass();

	RenderPassMain();
	Mtx44 projection;
	modelStack.LoadIdentity();
	viewStack.LoadIdentity();
	viewStack.LookAt(camera->position.x, camera->position.y, camera->position.z, camera->target.x, camera->target.y, camera->target.z, camera->up.x, camera->up.y, camera->up.z);
	projection.SetToPerspective(70, 16.f / 9.f, 0.1f, 10000.f);
	projectionStack.LoadMatrix(projection);

	glUniformMatrix4fv(m_parameters[U_VIEW], 1, GL_FALSE, &viewStack.Top().a[0]);
	glUniformMatrix4fv(m_parameters[U_PROJECTION], 1, GL_FALSE, &projection.a[0]);

	Vector3 lightDir(0, 1, 0);
	Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);

	glUniform1i(m_parameters[U_FOG_ENABLED], 0);
	RenderSkybox();
	glUniform1i(m_parameters[U_FOG_ENABLED], 1);
	RenderScene();
	glUniform1i(m_parameters[U_FOG_ENABLED], 0);

	viewStack.LoadIdentity();
	projection.SetToOrtho(0, Application::m_width, 0, Application::m_height, -80, 80);
	projectionStack.LoadMatrix(projection);

	glUniformMatrix4fv(m_parameters[U_VIEW], 1, GL_FALSE, &viewStack.Top().a[0]);
	glUniformMatrix4fv(m_parameters[U_PROJECTION], 1, GL_FALSE, &projection.a[0]);

	glDisable(GL_DEPTH_TEST);
	Render2D();
	glEnable(GL_DEPTH_TEST);
}

void MinScene::RenderEntities_GPass()
{
	Mesh * mesh[6] = {
		meshList["HEAD"],
		meshList["BODY"],
		meshList["L_ARM"],
		meshList["L_LEG"],
		meshList["R_ARM"],
		meshList["R_LEG"] };
	vector<Mtx44> MMat[6]; //Head, Body, Arm, Leg
	float rotation = sin(Math::RadianToDegree(elapsedTime * 0.1f)) * 30;

	unsigned count = LivingThings.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (LivingThings[i]->position.x >(int)camera->position.x + 47 || LivingThings[i]->position.x < (int)camera->position.x - 47 ||
			LivingThings[i]->position.z >(int)camera->position.z + 47 || LivingThings[i]->position.z < (int)camera->position.z - 47)
			continue;

		modelStack.PushMatrix();
		modelStack.Translate(LivingThings[i]->position);
		modelStack.Rotate(LivingThings[i]->hOrientation, 0, 1, 0);

		modelStack.PushMatrix();
		MMat[0].push_back(modelStack.Top());
		MMat[1].push_back(modelStack.Top());
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1.5f, 0);
		modelStack.Rotate(rotation, 1, 0, 0);
		modelStack.Translate(0, -1.5f, 0);
		MMat[2].push_back(modelStack.Top());
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(0, 0.75f, 0);
		modelStack.Rotate(rotation, 1, 0, 0);
		modelStack.Translate(0, -0.75f, 0);
		MMat[3].push_back(modelStack.Top());
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1.5f, 0);
		modelStack.Rotate(-rotation, 1, 0, 0);
		modelStack.Translate(0, -1.5f, 0);
		MMat[4].push_back(modelStack.Top());
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(0, 0.75f, 0);
		modelStack.Rotate(-rotation, 1, 0, 0);
		modelStack.Translate(0, -0.75f, 0);
		MMat[5].push_back(modelStack.Top());
		modelStack.PopMatrix();

		modelStack.PopMatrix();
	}

	for (unsigned i = 0; i < 6; ++i)
	{
		if (!MMat[i].empty())
			RenderInstance(mesh[i], MMat[i].size(), &MMat[i][0], true);
	}
}

void MinScene::RenderEntities()
{
	Vector3 dir = (camera->target - camera->position).Normalized();

	unsigned textureIDs[3] = { textureID["PLAYER2"],textureID["PLAYER3"], textureID["PLAYER4"] };
	Mesh * mesh[6] = {
		meshList["HEAD"],
		meshList["BODY"],
		meshList["L_ARM"],
		meshList["L_LEG"],
		meshList["R_ARM"],
		meshList["R_LEG"] };
	vector<Mtx44> MMat[3][6]; //Head, Body, Arm, Leg
	float rotation = sin(Math::RadianToDegree(elapsedTime * 0.1f)) * 30;

	unsigned count = LivingThings.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (!Block(LivingThings[i]->position, LivingThings[i]->collision.centre, LivingThings[i]->collision.hitbox).collisionWithCylinder(camera->position + dir * (RenderDist * 0.5f), RenderDist * 0.5f))
			continue;

		modelStack.PushMatrix();
		modelStack.Translate(LivingThings[i]->position);
		modelStack.Rotate(LivingThings[i]->hOrientation, 0, 1, 0);

		modelStack.PushMatrix();
		MMat[LivingThings[i]->id][0].push_back(modelStack.Top());
		MMat[LivingThings[i]->id][1].push_back(modelStack.Top());
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1.5f, 0);
		modelStack.Rotate(rotation, 1, 0, 0);
		modelStack.Translate(0, -1.5f, 0);
		MMat[LivingThings[i]->id][2].push_back(modelStack.Top());
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(0, 0.75f, 0);
		modelStack.Rotate(rotation, 1, 0, 0);
		modelStack.Translate(0, -0.75f, 0);
		MMat[LivingThings[i]->id][3].push_back(modelStack.Top());
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(0, 1.5f, 0);
		modelStack.Rotate(-rotation, 1, 0, 0);
		modelStack.Translate(0, -1.5f, 0);
		MMat[LivingThings[i]->id][4].push_back(modelStack.Top());
		modelStack.PopMatrix();
		modelStack.PushMatrix();
		modelStack.Translate(0, 0.75f, 0);
		modelStack.Rotate(-rotation, 1, 0, 0);
		modelStack.Translate(0, -0.75f, 0);
		MMat[LivingThings[i]->id][5].push_back(modelStack.Top());
		modelStack.PopMatrix();

		modelStack.PopMatrix();
	}

	for (unsigned j = 0; j < 3; ++j)
	{
		for (unsigned i = 0; i < 6; ++i)
		{
			mesh[i]->textureID = textureIDs[j];

			if (!MMat[j][i].empty())
				RenderInstance(mesh[i], MMat[j][i].size(), &MMat[j][i][0], true);
		}
	}
}

void MinScene::RenderBlocks_GPass()
{
	vector<Mtx44> MMat[2];
	unsigned count = blockList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (blockList[i]->position.x >(int)camera->position.x + 47 || blockList[i]->position.x < (int)camera->position.x - 48 ||
			blockList[i]->position.z >(int)camera->position.z + 47 || blockList[i]->position.z < (int)camera->position.z - 48)
			continue;

		modelStack.PushMatrix();
		blockList[i]->RenderObject(modelStack);
		if (blockList[i]->type == Block::STAIR)
			MMat[1].push_back(modelStack.Top());
		else
			MMat[0].push_back(modelStack.Top());
		modelStack.PopMatrix();
	}

	if (MMat[0].size() > 0)
		RenderInstance(meshList["BLOCK"], MMat[0].size(), &MMat[0][0]);
	if (MMat[1].size() > 0)
		RenderInstance(meshList["STAIR"], MMat[1].size(), &MMat[1][0]);
}

void MinScene::RenderBlocks()
{
	Vector3 dir = (camera->target - camera->position).Normalized();

	vector<Mtx44> MMat[Block::NUM_TYPES];
	vector<TexCoord> texOffset[Block::NUM_TYPES];
	
	unsigned count = blockList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (blockList[i]->type == Block::TRANS)
			continue;
		if (!blockList[i]->collisionWithCylinder(camera->position + dir * (RenderDist * 0.5f), RenderDist * 0.5f))
			continue;

		modelStack.PushMatrix();
		blockList[i]->RenderObject(modelStack);
		MMat[blockList[i]->type].push_back(modelStack.Top());
		texOffset[blockList[i]->type].push_back(Block::getTextureOffset(blockList[i]->id, blockList[i]->type));
		modelStack.PopMatrix();
	}

	count = alphaBlockList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (!alphaBlockList[i]->collisionWithCylinder(camera->position + dir * (RenderDist * 0.5f), RenderDist * 0.5f))
			continue;

		modelStack.PushMatrix();
		alphaBlockList[i]->RenderObject(modelStack);
		MMat[alphaBlockList[i]->type].push_back(modelStack.Top());
		texOffset[alphaBlockList[i]->type].push_back(Block::getTextureOffset(alphaBlockList[i]->id, alphaBlockList[i]->type));
		modelStack.PopMatrix();
	}

	glUniform1i(m_parameters[U_TEXTURE_ROWS], 4);
	if (MMat[0].size() > 0)
		RenderInstanceAtlas(meshList["BLOCK"], MMat[0].size(), &MMat[0][0], &texOffset[0][0]);
	if (MMat[2].size() > 0)
		RenderInstanceAtlas(meshList["STAIR"], MMat[2].size(), &MMat[2][0], &texOffset[2][0]);

	if (MMat[1].size() > 0)
	{
		glUniform1i(m_parameters[U_TEXTURE_ROWS], 6);
		RenderInstanceAtlas(meshList["MT_BLOCK"], MMat[1].size(), &MMat[1][0], &texOffset[1][0]);
	}
	if (MMat[3].size() > 0)
	{
		glUniform1i(m_parameters[U_TEXTURE_ROWS], 4);
		RenderInstanceAtlas(meshList["BLOCK"], MMat[3].size(), &MMat[3][0], &texOffset[3][0]);
	}

	glUniform1i(m_parameters[U_TEXTURE_ROWS], 0);
}

void MinScene::RenderScene()
{
	RenderBlocks();

	if (selectedBlock)
	{
		modelStack.PushMatrix();
		selectedBlock->RenderObject(modelStack);
		RenderMesh(meshList["WIREBLOCK"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
	}

	unsigned count = blockList.size();

	if (Application::IsKeyPressed('Q'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

		meshList["CUBE"]->textureID = NULL;

		glDisable(GL_CULL_FACE);
		modelStack.PushMatrix();
		modelStack.Translate(player.initialPos);
		modelStack.Scale(player.collision.hitbox);
		RenderMesh(meshList["CUBE"], false);
		modelStack.PopMatrix();
		glEnable(GL_CULL_FACE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode

		vector<Mtx44> MMat;

		for (unsigned i = 0; i < count; ++i)
		{
			if (blockList[i]->type != Block::STAIR)
			{
				if (blockList[i]->position.DistSquared(camera->position) < 16 * 16)
				{
					modelStack.PushMatrix();
					blockList[i]->RenderObject(modelStack);
					MMat.push_back(modelStack.Top());
					modelStack.PopMatrix();
				}
			}
		}

		RenderInstance(meshList["WIREBLOCK"], MMat.size(), &MMat[0], false);
	}

	count = particleList.size();
	if (count > 0)
	{
		vector<Mtx44> MMat[2];
		vector<TexCoord> texOffset[2];
		for (unsigned i = 0; i < count; i++)
		{
			modelStack.PushMatrix();
			modelStack.Translate(particleList[i].position);
			modelStack.Rotate(camera->orientation + 180, 0, 1, 0);
			modelStack.Rotate(camera->look, 1, 0, 0);
			modelStack.Scale(particleList[i].size);
			if (particleList[i].blockID > 3)
			{
				texOffset[0].push_back(Block::getTextureOffset(particleList[i].blockID));
				MMat[0].push_back(modelStack.Top());
			}
			else
			{
				texOffset[1].push_back(Block::getTextureOffset(particleList[i].blockID, Block::MT));
				MMat[1].push_back(modelStack.Top());
			}
			modelStack.PopMatrix();
		}

		glUniform1i(m_parameters[U_PARTICLE_TEXTURE], 1);
	
		if (MMat[0].size() > 0)
		{
			glUniform1i(m_parameters[U_TEXTURE_ROWS], 4);
			meshList["QUAD"]->textureID = meshList["BLOCK"]->textureID;
			RenderInstanceAtlas(meshList["QUAD"], MMat[0].size(), &MMat[0][0], &texOffset[0][0], false, Color(0.75f, 0.75f, 0.75f));
			meshList["QUAD"]->textureID = NULL;
		}
		if (MMat[1].size() > 0)
		{
			glUniform1i(m_parameters[U_TEXTURE_ROWS], 6);
			meshList["QUAD"]->textureID = meshList["MT_BLOCK"]->textureID;
			RenderInstanceAtlas(meshList["QUAD"], MMat[1].size(), &MMat[1][0], &texOffset[1][0], false, Color(0.75f, 0.75f, 0.75f));
			meshList["QUAD"]->textureID = NULL;
		}

		glUniform1i(m_parameters[U_PARTICLE_TEXTURE], 0);
		glUniform1i(m_parameters[U_TEXTURE_ROWS], 0);
	}

	RenderEntities();
}

void MinScene::Render2D()
{
	ostringstream ss;

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.5f, 0);

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f - 100, Application::m_height * 0.5f - 100, 0);
	modelStack.Rotate(-camera->orientation, 0, 0, 1);

	meshList["CIRCLE"]->textureID = m_lightDepthFBO.GetTexture();
	modelStack.PushMatrix();
	modelStack.Scale(180, 180, 1);
	RenderMesh(meshList["CIRCLE"], false, Color(0,1,0));
	modelStack.PopMatrix();
	meshList["CIRCLE"]->textureID = NULL;

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(0, 80, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(16);
	modelStack.Translate(-Application::getTextWidth("N") * 0.5f, 0, 0);
	ss << "N";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(0, -80, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(16);
	modelStack.Translate(-Application::getTextWidth("S") * 0.5f, 0, 0);
	ss << "S";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(-80, 0, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(16);
	modelStack.Translate(-Application::getTextWidth("W") * 0.5f, 0, 0);
	ss << "W";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(80, 0, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(16);
	modelStack.Translate(-Application::getTextWidth("E") * 0.5f, 0, 0);
	ss << "E";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	glUniform1f(m_parameters[U_ALPHA], 0.75f);
	modelStack.PushMatrix();
	modelStack.Scale(2.5f, 20, 1);
	RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Scale(2.5f, 20, 1);
	RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
	modelStack.PopMatrix();

	if (shouldRenderChat)
	{
		modelStack.PushMatrix();
		modelStack.Translate(0, -Application::m_height * 0.5f + 25, 0);

		modelStack.PushMatrix();
		modelStack.Scale(Application::m_width, 30, 1);
		RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
		glUniform1f(m_parameters[U_ALPHA], 1);

		modelStack.PushMatrix();
		modelStack.Translate(-Application::m_width * 0.5f + 6, 0, 0);
		modelStack.Scale(25);
		if ((int)(elapsedTime * 2) % 2 == 0)
			RenderText(line, Color(1, 1, 1));
		else
			RenderText(line + "_", Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.PopMatrix();
	}

	modelStack.PopMatrix();
	glUniform1f(m_parameters[U_ALPHA], 1);

	modelStack.PushMatrix();
	modelStack.Translate(6, 650, 0);
	modelStack.Scale(24);
	string text = "Min Build 0.1f";
	if (player.noClip)
		text += " noClip";
	RenderText(text, Color(1, 1, 1));

	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 615, 0);
	modelStack.Scale(20);
	ss.str("");
	ss << setprecision(30) << "x " << player.position.x;
	RenderText(ss.str(), Color(1, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 595, 0);
	modelStack.Scale(20);
	ss.str("");
	if (player.position.y <= Math::EPSILON && -player.position.y <= Math::EPSILON)
		ss << "y " << 0;
	else
		ss << "y " << player.position.y;
	RenderText(ss.str(), Color(0, 1, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 575, 0);
	modelStack.Scale(20);
	ss.str("");
	ss << "z " <<  player.position.z;
	RenderText(ss.str(), Color(0, 0, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(6, 540, 0);
	modelStack.Scale(20);
	ss.str("");
	ss.precision(5);
	ss << (int)(fps + 0.5f) << " fps (" << (int)(minFPS + 0.5f) << "/" << (int)(peakFPS + 0.5f) << ") (" << blockList.size() << "/" << worldBlocks << " blocks)";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	glUniform1f(m_parameters[U_ALPHA], 1);

	Mtx44 selectorMMat;
	vector<Mtx44>MMat[Block::NUM_TYPES + 1];
	vector<TexCoord>texOffset[Block::NUM_TYPES];
	
	for (unsigned i = 0; i < blockInventory.size; ++i)
	{
		modelStack.PushMatrix();
		modelStack.Translate(Application::m_width * (i * 0.036f + 0.0275f), Application::m_height * 0.05f, 0);

		modelStack.PushMatrix();
		modelStack.Scale(28);
		MMat[Block::NUM_TYPES].push_back(modelStack.Top());
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Rotate(25, 1, 0, 0);
		modelStack.Rotate(45, 0, 1, 0);
		if (&blockInventory.getBlock() == &blockInventory.getBlock(i))
			modelStack.Scale(1.5f);
		modelStack.Scale(24);
		modelStack.Translate(-1, 0.125f, 0);

		MMat[blockInventory.getBlock(i).type].push_back(modelStack.Top());
		texOffset[blockInventory.getBlock(i).type].push_back(Block::getTextureOffset(blockInventory.getBlock(i).id, blockInventory.getBlock(i).type));

		modelStack.PopMatrix();

		modelStack.PopMatrix();
	}

	meshList["QUAD"]->textureID = textureID["SLOT"];
	RenderInstance(meshList["QUAD"], MMat[Block::NUM_TYPES].size(), &MMat[Block::NUM_TYPES][0], false);
	meshList["QUAD"]->textureID = NULL;

	glUniform1i(m_parameters[U_TEXTURE_ROWS], 4);
	if (MMat[0].size() > 0)
		RenderInstanceAtlas(meshList["BLOCK"], MMat[0].size(), &MMat[0][0], &texOffset[0][0]);
	if (MMat[2].size() > 0)
		RenderInstanceAtlas(meshList["STAIR"], MMat[2].size(), &MMat[2][0], &texOffset[2][0]);

	if (MMat[1].size() > 0)
	{
		glUniform1i(m_parameters[U_TEXTURE_ROWS], 6);
		RenderInstanceAtlas(meshList["MT_BLOCK"], MMat[1].size(), &MMat[1][0], &texOffset[1][0]);
	}
	if (MMat[3].size() > 0)
	{
		glUniform1i(m_parameters[U_TEXTURE_ROWS], 4);
		RenderInstanceAtlas(meshList["BLOCK"], MMat[3].size(), &MMat[3][0], &texOffset[3][0]);
	}
}

void MinScene::writeLine()
{
	for (unsigned i = '0'; i <= '9'; ++i)
	{
		if (Application::IsKeyPressed(i))
		{
			chatInputLimiter = 0.f;
			line += i;

			return;
		}
	}

	for (unsigned i = 'A'; i <= 'Z'; ++i)
	{
		if (Application::IsKeyPressed(i))
		{
			chatInputLimiter = 0.f;

			if (Application::IsKeyPressed(VK_SHIFT))
				line += i;
			else
				line += i + 32;

			return;
		}
	}

	if (Application::IsKeyPressed(VK_SPACE))
	{
		chatInputLimiter = 0.f;
		line += " ";
		return;
	}
	if (Application::IsKeyPressed(VK_BACK) && line.size() > 1)
	{
		chatInputLimiter = 0.f;
		line.pop_back();
		return;
	}
}

bool MinScene::Save(const char * filepath)
{
	ofstream outputFile;
	outputFile.open(filepath);

	if (outputFile.is_open())
	{
		outputFile << player.position << " " << camera->orientation << " " << camera->look << endl;

		for (unsigned z = 0; z < worldZ; ++z)
		{
			for (unsigned y = 0; y < worldY; ++y)
			{
				for (unsigned x = 0; x < worldX; ++x)
				{
					if (worldBlockList[x][y][z])
						outputFile << *worldBlockList[x][y][z];
				}
			}
		}

		outputFile.close();

		std::cout << "Saved to " << filepath << endl;

		return true;
	}

	std::cout << "Failed to save to " << filepath << endl;

	return false;
}

bool MinScene::Load(const char * filepath)
{
	int lineCount = 1;
	string line;

	ifstream inputFile;
	inputFile.open(filepath);

	if (inputFile.is_open())
	{
		vector<float> data;

		while (getline(inputFile, line))
		{
			char *dup = strdup(line.c_str());
			char * token = strtok(dup, " [,]");

			while (token != NULL)
			{
				data.push_back(atof(token));
				token = strtok(NULL, " [,]");
			}

			if (lineCount == 1)
			{
				player.position.Set(data[0], data[1], data[2]);
				if (camera)
				{
					camera->orientation = data[3]; camera->look = data[4];
				}
			}
			else if (data.size() == 5)
			{
				FetchBlock(Vector3(data[0], data[1], data[2]), false, data[3], (Block::blockType)(int)data[4]);
			}
			else
			{
				FetchStair(Vector3(data[0], data[1], data[2]), false, data[3], data[5], data[6]);
			}
			free(dup);
			data.clear();
			lineCount++;
		}

		inputFile.close();

		std::cout << "Loaded " << filepath << endl;

		return true;
	}

	std::cout << "Failed to load" << filepath << endl;

	return false;
}

bool MinScene::Convert(const char * filepath)
{
	int lineCount = 1;
	string line;
	vector<string> data;

	ifstream inputFile;
	inputFile.open(filepath);

	if (inputFile.is_open())
	{
		while (getline(inputFile, line))
		{
			if (lineCount > 1)
				line += " 1";
			data.push_back(line);
			lineCount++;
		}

		inputFile.close();
	}

	ofstream outputFile;
	outputFile.open(filepath);

	if (outputFile.is_open())
	{
		for (unsigned i = 0; i < data.size(); ++i)
			outputFile << data[i] << endl;
		outputFile.close();

		return true;
	}

	return false;
}

bool MinScene::GenerateWorld(Vector3 size)
{
	vector<Vector3> terrainRise;

	int y = worldY*0.5f - size.y*0.5f;
	for (unsigned z = 0; z < size.z; ++z)
	{
		for (unsigned x = 0; x < size.x; ++x)
		{
			FetchBlock(Vector3(x, y, z), false, 0); //Create a flat piece of land

			if (rand() % 90 == 0)
				terrainRise.push_back(Vector3(x, y + 1, z)); //Randomize hilly points
		}
	}

	for (unsigned i = 0; i < terrainRise.size(); ++i) //Generate the hills
	for (unsigned z = 0; z < size.z; ++z)
	{
		for (unsigned x = 0; x < size.x; ++x)
		{
			Vector3 pos(x, terrainRise[i].y, z);
			int random = rand() % 3 + 3; //Randomize hill size

			if (terrainRise[i].DistSquared(pos) < random*random)
			{
				if (FetchBlock(pos, true, 0)) //Cull block below hill
				{
					delete worldBlockList[x][(int)terrainRise[i].y - 1][z];
					worldBlockList[x][(int)terrainRise[i].y - 1][z] = NULL;
					--worldBlocks;
				}
			}
		}
	}

	y++;
	for (unsigned z = 1; z < size.z - 1; ++z)
	{
		for (unsigned x = 1; x < size.x - 1; ++x)
		{
			if (!worldBlockList[x][y][z]) //Fill in holes
			{
				unsigned count = 0;

				if (worldBlockList[x + 1][y][z])
					count++;
				if (worldBlockList[x - 1][y][z])
					count++;
				if (worldBlockList[x][y][z + 1])
					count++;
				if (worldBlockList[x][y][z - 1])
					count++;
				
				if (count >= 3)
				{
					if (FetchBlock(Vector3(x, y, z), false, 0))
					{
						delete worldBlockList[x][y - 1][z];
						worldBlockList[x][y - 1][z] = NULL;
						--worldBlocks;
					}
				}
			}
		}
	}

	for (unsigned z = 0; z < size.z; ++z)
	{
		for (unsigned x = 0; x < size.x; ++x)
		{
			if (worldBlockList[x][y][z])
			{
				if (rand() % 256 == 0)
				{
					delete worldBlockList[x][y - 1][z];
					worldBlockList[x][y - 1][z] = NULL;
					--worldBlocks;

					for (unsigned i = 1; i <= 6; ++i)
					{
						FetchBlock(Vector3(x, y + i, z), false, 2);

						if (i >= 6)
						{
							for (int X = x - 1; X <= x + 1; ++X)
							{
								for (int Z = z - 1; Z <= z + 1; ++Z)
								{
									FetchBlock(Vector3(X, y + i, Z), false, 13, Block::TRANS);
								}
							}
						}
						else if (i >= 4)
						{
							for (int X = x - 2; X <= x + 2; ++X)
							{
								for (int Z = z - 2; Z <= z + 2; ++Z)
								{
									if (i == 4)
									{
										if (X == x - 2 && Z == z - 2)
											continue;
										if (X == x - 2 && Z == z + 2)
											continue;
										if (X == x + 2 && Z == z + 2)
											continue;
										if (X == x + 2 && Z == z - 2)
											continue;
									}
									FetchBlock(Vector3(X, y + i, Z), false, 13, Block::TRANS);
								}
							}
						}
					}

					FetchBlock(Vector3(x, y + 7, z), false, 13, Block::TRANS);
					FetchBlock(Vector3(x + 1, y + 7, z), false, 13, Block::TRANS);
					FetchBlock(Vector3(x - 1, y + 7, z), false, 13, Block::TRANS);
					FetchBlock(Vector3(x, y + 7, z + 1), false, 13, Block::TRANS);
					FetchBlock(Vector3(x, y + 7, z - 1), false, 13, Block::TRANS);
				}
			}
		}
	}

	return true;
}

void MinScene::RenderSkybox()
{
	meshList["QUAD"]->textureID = textureID["SKYBOX"];
	glUniform1i(m_parameters[U_TEXTURE_ROWS], 3);

	vector<Mtx44>MMat;
	vector<TexCoord>offset;

	modelStack.PushMatrix();
//	modelStack.Translate(camera->position);
	modelStack.Rotate(elapsedTime * 0.5f, 0, 180, 0);
	modelStack.Scale(500);

	modelStack.PushMatrix(); //UP
	modelStack.Translate(0, 0.4985f, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((0 % 3) / 3.f, 1 - ((0 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //DOWN
	modelStack.Translate(0, -0.4985f, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((1 % 3) / 3.f, 1 - ((1 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //LEFT
	modelStack.Translate(0.4985f, 0, 0);
	modelStack.Rotate(-90, 0, 1, 0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((2 % 3) / 3.f, 1 - ((2 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //RIGHT
	modelStack.Translate(-0.4985f,0,0);
	modelStack.Rotate(90,0,1,0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((3 % 3) / 3.f, 1 - ((3 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //FRONT
	modelStack.Translate(0, 0, -0.4985f);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((4 % 3) / 3.f, 1 - ((4 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //BACK
	modelStack.Translate(0, 0, 0.4985f);
	modelStack.Rotate(180, 0, 1, 0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((5 % 3) / 3.f, 1 - ((5 / 3) + 1) / 3.f));
	modelStack.PopMatrix();
	
	modelStack.PopMatrix();

	RenderInstanceAtlas(meshList["QUAD"], MMat.size(), &MMat[0], &offset[0], false);

	glUniform1i(m_parameters[U_TEXTURE_ROWS], 0);
	meshList["QUAD"]->textureID = NULL;
}

void MinScene::Exit()
{
	soundExit();

	while (particleList.size() > 0)
	{
		particleList.pop_back();
	}

	for (unsigned z = 0; z < worldZ; ++z)
	{
		for (unsigned y = 0; y < worldY; ++y)
		{
			for (unsigned x = 0; x < worldX; ++x)
			{
				if (worldBlockList[x][y][z])
					delete worldBlockList[x][y][z];
			}
		}
	}

	SceneBase::Exit();
}