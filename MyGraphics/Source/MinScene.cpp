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

MinScene::MinScene() :selectedBlock(NULL), nextUpdate(0)
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
	blockInventory.Init();
	RenderDist = 50;

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

	//Load("Save//Winterfell.txt");
	LoadOutpost();
	LoadLionsDen();
	LoadWinterfell();
	LoadCastleBlack();
	GenerateWorld(Vector3(worldX, 1, worldZ));

	cout << worldBlocks <<  " Objects Initialized." << endl << endl;

	player.position.Set(0, 0, 0);
	camera->position = player.position; camera->position.y += player.eyeLevel;
	soundInit();

	minShadowCoord.Set(-48, -24, -48);
	maxShadowCoord.Set(48, 24, 48);
	showMap = false;

	for (unsigned i = 0; i < 256; ++i)
	{
		unsigned id = rand() % NumEntities;

		switch (id)
		{
		case Entity::ENEMY_1: case Entity::ENEMY_2: case Entity::ENEMY_3:
			{
				Enemy* entity = new Enemy(2, 4.0, 3.0);
				entity->position.Set(rand() % 101 - 50, 1, rand() % 101 - 50);
				entity->hOrientation = rand() % 360;
				entity->collision.hitbox.Set(0.6f, 1.8f, 0.6f);
				entity->collision.centre.Set(0, 0.9f, 0);
				entity->entityID = id;
				worldLivingThings[id].push_back(entity);
				break;
			}
		case Entity::HORSE:
			{
				Entity* entity = new Entity();
				entity->position.Set(rand() % 101 - 50, 1, rand() % 101 - 50);
				entity->hOrientation = rand() % 360;
				entity->collision.hitbox.Set(1.4f, 2.1f, 1.4f);
				entity->collision.centre.Set(0, 1.05f, 0);
				entity->entityID = id;
				worldLivingThings[id].push_back(entity);
				break;
			}
		case Entity::WOLF:
			{
				Living* entity = new Living();
				entity->position.Set(rand() % 101 - 50, 1, rand() % 101 - 50);
				entity->hOrientation = entity->newOrientation = rand() % 360;
				entity->collision.hitbox.Set(0.7f, 0.8f, 0.7f);
				entity->collision.centre.Set(0, 0.4f, 0);
				entity->entityID = id;
				entity->health = 50;
				worldLivingThings[id].push_back(entity);
				break;
			}
		case Entity::DROP:
		{
			Entity* entity = FetchEntity(Entity::DROP);
			entity->position.Set(rand() % 101 - 50, 1, rand() % 101 - 50);
			break;
		}
		default:
			break;
		}
	}

	InTheZone = false;
	ZoneBar = MaxZoneTime = 12;

	CBow* Bow = new CBow();
	Bow->mesh = meshList["BOW"];
	player.inventory.slot[1]->item = Bow;

	CKnife* Knife = new CKnife();
	Knife->mesh = meshList["KNIFE"];
	player.inventory.slot[0]->item = Knife;

	CFood* Food = new CFood();
	Food->mesh = meshList["MEAT"];
	player.inventory.slot[2]->item = Food;

	Waypoint w;
	w.point.Set(93, 0);
	w.name = "Castle Black";
	w.texture = textureID["MAP_CASTLE"]; w.color.Set(0, 0, 0);
	waypointList.push_back(w);
	w.point.Set(-18, 45);
	w.name = "Lion's Den";
	w.texture = textureID["MAP_LION"]; w.color.Set(1, 1, 1);
	waypointList.push_back(w);
	w.point.Set(0, -82);
	w.name = "Ruined Outpost";
	w.texture = textureID["MAP_CASTLE"]; w.color.Set(0.5f, 0.5f, 0.5f);
	waypointList.push_back(w);
	w.point.Set(-92, 0);
	w.name = "Winterfell";
	w.texture = textureID["MAP_CASTLE"]; w.color.Set(1, 1, 1);
	waypointList.push_back(w);

	playerWaypoint.point.Set(player.position.x, player.position.z);
	playerWaypoint.name = "Player";
	playerWaypoint.texture = textureID["MAP_PLAYER"];
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

		for (unsigned j = 0; j < NumEntities; ++j)
		{
			unsigned count = LivingThings[j].size();
			for (unsigned i = 0; i < count; ++i)
			{
				if (Block::checkCollision(*o, Block(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox)))
				{
					delete o;
					return false;
				}
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
		c.hitbox.Set(1, 0.5f, 1);
		if (look == 90)
			c.centre.Set(0.5f, 0.75f, 0.5f);
		else
			c.centre.Set(0.5f, 0.25f, 0.5f);
		o->collisions.push_back(c);
		c.hitbox.Set(1, 1, 0.5f);
		c.centre.Set(0.5f, 0.5f, 0.25f);
		o->collisions.push_back(c);
		break;
	case 180:
		c.hitbox.Set(1, 0.5f, 1);
		if (look == 90)
			c.centre.Set(0.5f, 0.75f, 0.5f);
		else
			c.centre.Set(0.5f, 0.25f, 0.5f);
		o->collisions.push_back(c);
		c.hitbox.Set(1, 1, 0.5f);
		c.centre.Set(0.5f, 0.5f, 0.75f);
		o->collisions.push_back(c);
		break;
	case 90:
		c.hitbox.Set(1, 0.5f, 1);
		if (look == 90)
			c.centre.Set(0.5f, 0.75f, 0.5f);
		else
			c.centre.Set(0.5f, 0.25f, 0.5f);
		o->collisions.push_back(c);
		c.hitbox.Set(0.5f, 1, 1);
		c.centre.Set(0.25f, 0.5f, 0.5f);
		o->collisions.push_back(c);
		break;
	case -90:
		c.hitbox.Set(1, 0.5f, 1);
		if (look == 90)
			c.centre.Set(0.5f, 0.75f, 0.5f);
		else
			c.centre.Set(0.5f, 0.25f, 0.5f);
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

		for (unsigned j = 0; j < NumEntities; ++j)
		{
			unsigned count = LivingThings[j].size();
			for (unsigned i = 0; i < count; ++i)
			{
				if (Block::checkCollision(Blk1, Block(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox)) ||
					Block::checkCollision(Blk2, Block(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox)))
				{
					delete o;
					return false;
				}
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
	for (unsigned i = 0; i < 255; ++i)
	{
		if (key == i)
			player.myKeys[i] = true;
	}
}

Vector3 MinScene::PositionToIndex(Vector3 pos)
{
	return Vector3(pos.x + worldX*0.5f, pos.y + worldY*0.5f, pos.z + worldZ*0.5f);
}

void MinScene::ObtainBlockList()
{	
	for (unsigned j = 0; j < NumEntities; ++j)
	{
		LivingThings[j].clear();

		unsigned count = worldLivingThings[j].size();
		for (unsigned i = 0; i < count; ++i)
		{
			if (!worldLivingThings[j][i]->IsActive())
				continue;

			if (camera->position.DistSquared(worldLivingThings[j][i]->position) < RenderDist * RenderDist)
				LivingThings[j].push_back(worldLivingThings[j][i]);
		}
	}

	alphaBlockList.clear();
	blockList.clear();

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
					}
				}
			}
		}
	}

	sort(alphaBlockList.begin(), alphaBlockList.end(), IsMoreThan);
}

void MinScene::PartitionCollision()
{
	for (unsigned j = 0; j < NumEntities; ++j)
	{
		unsigned count = LivingThings[j].size();
		for (unsigned i = 0; i < count; ++i)
		{
			LivingThings[j][i]->collisionBlockList.clear();

			if (LivingThings[j][i]->IsActive() && !LivingThings[j][i]->IsDead())
			{
				Vector3 Position = PositionToIndex(LivingThings[j][i]->position);
				for (int x = Position.x - 1; x < Position.x + 1; ++x)
				{
					for (int y = Position.y - 3; y < Position.y + 3; ++y)
					{
						for (int z = Position.z - 1; z < Position.z + 1; ++z)
						{
							if (x >= 0 && x < worldX && y >= 0 && y < worldY && z >= 0 && z < worldZ)
							{
								if (worldBlockList[x][y][z])
									LivingThings[j][i]->collisionBlockList.push_back(worldBlockList[x][y][z]);
							}
						}
					}
				}
			}
		}
	}

	player.collisionBlockList.clear();
	Vector3 Position = PositionToIndex(player.position);
	for (int x = Position.x - 1; x < Position.x + 1; ++x)
	{
		for (int y = Position.y - 3; y < Position.y + 3; ++y)
		{
			for (int z = Position.z - 1; z < Position.z + 1; ++z)
			{
				if (x >= 0 && x < worldX && y >= 0 && y < worldY && z >= 0 && z < worldZ)
				{
					if (worldBlockList[x][y][z])
						player.collisionBlockList.push_back(worldBlockList[x][y][z]);
				}
			}
		}
	}
}

void MinScene::Update_LevelEditor(double dt)
{
	blockInventory.Update();

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

	static bool bRMouseButtonPressed = false;
	static float bRMouseButtonElapsed = 0.f;

	if (!bRMouseButtonPressed && Application::IsMousePressed(1))
	{
		bRMouseButtonElapsed = 0.f;
		bRMouseButtonPressed = true;

		if (selectedBlock)
		{
			bool playSound = false;
			Vector3 view = (camera->target - camera->position).Normalized();

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
					UpperBlock.position.y += 0.5f;
					Block LowerBlock(selectedBlock->position, selectedBlock->collision.centre, selectedBlock->collision.hitbox);
					LowerBlock.position.y -= 0.5f;

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

				selectedBlock = NULL;
			}
		}
	}
	else if ((bLMouseButtonPressed && !Application::IsMousePressed(0)))
	{
		bLMouseButtonPressed = false;
	}
}
void MinScene::Update_Game(double dt)
{
	if (InTheZone)
		player.inventory.Update(dt * 4);
	else
		player.inventory.Update(dt);

	if (player.getSelectedItem())
	{
		if (player.getSelectedItem()->use)
		{
			switch (player.getSelectedItem()->itemID)
			{
			case CItem::BOW:
			{
				GenerateArrow(player, (player.inventory.selectedSlot->item->getCharge() - 0.4f) * 64);
				char* soundFileLocation[3] = { "Assets/Media/Weapons/shoot1.mp3", "Assets/Media/Weapons/shoot2.mp3" , "Assets/Media/Weapons/shoot3.mp3" };
				ISound* sound = engine->play2D(soundFileLocation[rand() % 3], false, true);
				if (sound)
				{
					sound->setVolume(0.4f);
					sound->setIsPaused(false);
				}
				break;
			}
			case CItem::KNIFE:
				if (selectedEntity)
				{
					selectedEntity->Knockback((selectedEntity->position - camera->position) * 10);
					for (unsigned l = 0; l < 5; ++l)
						SpawnParticle(selectedEntity->position, 15);

					if (selectedEntity->health <= 0) //If the entity dies
					{
						for (unsigned l = 0; l < 10; ++l)
							SpawnParticle(selectedEntity->position, 15); //Generate more blood
						
						selectedEntity->ClearArrows();
						selectedEntity->SetActive(false);

						for (unsigned i = 0; i < 10; ++i)
						{
							Entity* meat = FetchEntity(Entity::DROP);
							meat->position = selectedEntity->position;
							meat->velocity.Set(rand() % 21 - 10, rand() % 16 + 5, rand() % 21 - 10);
						}

						unsigned count = LivingThings[selectedEntity->entityID].size();
						for (unsigned i = 0; i < count; ++i)
						{
							if (LivingThings[selectedEntity->entityID][i] == selectedEntity)
								LivingThings[selectedEntity->entityID].erase(LivingThings[selectedEntity->entityID].begin() + i);
						}

						selectedEntity = NULL;
					}
				}
			}

			player.getSelectedItem()->use = false;
			player.getSelectedItem()->setCharge(0);
		}
	}
	
	static bool bLButtonPressed = false;

	if (!bLButtonPressed && Application::IsKeyPressed('L'))
	{
		bLButtonPressed = true;

		if (!InTheZone && ZoneBar == MaxZoneTime)
		{
			ISound* sound = engine->play2D("Assets/Media/unravel.wav", false, true);
			if (sound)
			{
				sound->setVolume(0.2f);
				sound->setIsPaused(false);
			}
			sound = engine->play2D("Assets/Media/SlowMoIn.mp3", false, true);
			if (sound)
			{
				sound->setVolume(0.3f);
				sound->setIsPaused(false);
			}
			InTheZone = true;
		}
	}
	else if (bLButtonPressed && !Application::IsKeyPressed('L'))
		bLButtonPressed = false;
}
void MinScene::Update(double dt)
{
	SceneBase::Update(dt);

	static bool bMButtonPressed = false;

	if (!bMButtonPressed && Application::IsKeyPressed('M'))
	{
		bMButtonPressed = true;

		if (showMap)
		{
			Application::HideCursor(true);
			minShadowCoord.Set(-48, -24, -48);
			maxShadowCoord.Set(48, 24, 48);
			showMap = false;
		}
		else
		{
			Application::HideCursor(false);
			Application::ResetCursorPos();
			lights[0].position.Set(0, 4, 0);
			minShadowCoord.Set(-128, -24, -128);
			maxShadowCoord.Set(128, 24, 128);
			showMap = true;
		}
	}
	else if (bMButtonPressed && !Application::IsKeyPressed('M'))
		bMButtonPressed = false;

	if (showMap)
	{
		elapsedTime += dt;
		double x = 0;
		double y = 0;
		Application::GetCursorPos(&x, &y); y = Application::m_height - y;

		for (unsigned i = 0; i < waypointList.size(); ++i)
		{
			Vector3 thePoint = waypointList[i].getWaypoint(worldX, worldZ, Application::m_height, Application::m_height);
			if (x < thePoint.x + 16 && x > thePoint.x - 16 && y < thePoint.y + 16 && y > thePoint.y - 16)
			{
				waypointList[i].selected = true;
				return;
			}
		}
		
		Vector3 thePoint = playerWaypoint.getWaypoint(worldX, worldZ, Application::m_height, Application::m_height);
		if (x < thePoint.x + 16 && x > thePoint.x - 16 && y < thePoint.y + 16 && y > thePoint.y - 16)
			playerWaypoint.selected = true;

		return;
	}

		ObtainBlockList();
	PartitionCollision();

	player.Update(dt, false);
	soundUpdate(player);
	playerWaypoint.point.Set(player.position.x, player.position.z);
	lights[0].position.Set(camera->position.x, 4, camera->position.z);

	if (InTheZone)
	{
		ZoneBar -= dt;
		dt *= 0.25f;

		if (ZoneBar < 0)
		{
			InTheZone = false;
			ZoneBar = 0.f;

			ISound* sound = engine->play2D("Assets/Media/SlowMoOut.mp3", false, true);
			if (sound)
			{
				sound->setVolume(0.25f);
				sound->setIsPaused(false);
			}
		}

		Color color(0.65f, 0.65f, 1);
		glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 1);
		glUniform3fv(m_parameters[U_COLOR_SCALE], 1, &color.r);
	}
	else
	{
		ZoneBar += dt;
		ZoneBar = ZoneBar > MaxZoneTime ? MaxZoneTime : ZoneBar;
		glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
	}

	static bool bNButtonPressed = false;

	if (!bNButtonPressed && Application::IsKeyPressed(VK_TAB))
	{
		if (!InTheZone)
			player.noClip = player.noClip ? false : true;
		bNButtonPressed = true;
	}
	else if (bNButtonPressed && !Application::IsKeyPressed(VK_TAB))
		bNButtonPressed = false;

	for (unsigned j = 0; j < NumEntities; ++j)
	{
		unsigned count = LivingThings[j].size();

		for (int i = 0; i < count; ++i)
		{
			if (LivingThings[j][i]->IsDead() || !LivingThings[j][i]->IsActive())
				continue;

			LivingThings[j][i]->Update(dt, false);

			if (j == 4) //If Arrows,
			{
				Block arrow(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox);

				if (LivingThings[j][i]->IsDead()) 
				{
					unsigned count2 = LivingThings[j][i]->collisionBlockList.size();
					for (unsigned k = 0; k < count2; ++k)
					{
						if (Block::checkCollision(*LivingThings[j][i]->collisionBlockList[k], arrow)) //When collide with block,
						{
							for (unsigned l = 0; l < 5; ++l)
								SpawnParticle(LivingThings[j][i]->position, LivingThings[j][i]->collisionBlockList[k]->id); //Spawn particles
						}
					}
					continue;
				}
			
				for (unsigned j2 = 0; j2 < NumEntities; ++j2) //Check with the other entities
				{
					if (j2 == Entity::ARROW || j2 == Entity::DROP) //Which are not arrows/drops
						continue;

					bool collided = false; //breaking case
					unsigned count2 = LivingThings[j2].size();

					for (unsigned k = 0; k < count2; ++k)
					{
						Block NPC(LivingThings[j2][k]->position, LivingThings[j2][k]->collision.centre, LivingThings[j2][k]->collision.hitbox);

						if (Block::checkCollision(arrow, NPC)) //If arrow collide with entity
						{
							LivingThings[j2][k]->aggro = &player;
							LivingThings[j2][k]->Knockback(LivingThings[j][i]->velocity); //Knockback entity
							for (unsigned l = 0; l < 10; ++l) //Generate Blood
								SpawnParticle(LivingThings[j][i]->position, 15);

							Arrow* StuckedArrow = dynamic_cast<Arrow*>(LivingThings[j][i]); //Stuck arrow to entity
							StuckedArrow->relativeOrientation = LivingThings[j2][k]->hOrientation;
							StuckedArrow->relativePosition = LivingThings[j][i]->position - LivingThings[j2][k]->position;
							LivingThings[j2][k]->StuckedArrows.push_back(StuckedArrow); //Add arrow to internal list
							LivingThings[j][i]->SetDead(true); 

							if (LivingThings[j2][k]->health <= 0) //If the entity dies
							{
								for (unsigned l = 0; l < 10; ++l)
									SpawnParticle(LivingThings[j][i]->position, 15); //Generate more blood

								LivingThings[j2][k]->ClearArrows();
								LivingThings[j2][k]->SetActive(false);

								for (unsigned i = 0; i < 10; i++)
								{
									Entity* meat = FetchEntity(Entity::DROP);
									meat->position = LivingThings[j2][k]->position;
									meat->velocity.Set(rand() % 21 - 10, rand() % 16 + 5, rand() % 21 - 10);
								}

								LivingThings[j2].erase(LivingThings[j2].begin() + k);
							}

							collided = true;
							break;
						}
					}

					if (collided)
					{
						LivingThings[j].erase(LivingThings[j].begin() + i); //Remove from list

						count2 = worldLivingThings[j].size();
						for (unsigned k = 0; k < count2; ++k)
						{
							if (worldLivingThings[j][k] == LivingThings[j][i])
							{
								worldLivingThings[j].erase(worldLivingThings[j].begin() + k); //Remove from world list
								break;
							}
						}

						--i;
						--count;
						break;
					}
				}
			}
		}
	}

	tooltip.clear();
	selectedEntity = NULL;
	selectedBlock = NULL;

	float dist = INT_MAX;
	Vector3 view = (camera->target - camera->position).Normalized();

	for (unsigned j = 0; j < NumEntities; ++j)
	{
		if (j == Entity::ARROW)
			continue;

		unsigned count = LivingThings[j].size();
		for (unsigned i = 0; i < count; ++i)
		{
			Block Blk(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox);
			float rayDist = Blk.collisionWithRay(camera->position, view);
			if (rayDist >= 0 && rayDist <= 2.5f)
			{
				if (rayDist < dist)
				{
					selectedEntity = LivingThings[j][i];
					dist = rayDist;
				}
			}
		}
	}

	unsigned count = blockList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		float rayDist = blockList[i]->collisionWithRay(camera->position, view);
		if (rayDist >= 0 && rayDist <= 5)
		{
			if (rayDist < dist)
			{
				selectedBlock = blockList[i]; selectedEntity = NULL;
				dist = rayDist;
			}
		}
	}

	static bool bEButtonPressed = false;

	if (!bEButtonPressed && Application::IsKeyPressed('E'))
	{
		bEButtonPressed = true;

		if (player.mount)
		{
			player.mount->climbHeight = 0.5f;
			player.mount = NULL;
		}
		else if (selectedEntity)
		{
			switch (selectedEntity->entityID)
			{
			case Entity::HORSE:
			{
				if (!player.mount)
				{
					char* soundFileLocation[3] = { "Assets/Media/Horse/horseRide1.mp3", "Assets/Media/Horse/horseRide2.mp3" , "Assets/Media/Horse/horseRide3.mp3" };
					ISound* sound = engine->play2D(soundFileLocation[rand() % 3], false, true);
					if (sound)
					{
						sound->setVolume(0.7f);
						sound->setIsPaused(false);
					}

					player.mount = selectedEntity;
					player.mount->climbHeight = 1;
				}
				break;
			}
			case Entity::DROP:
			{
				selectedEntity->SetActive(false);
				/*unsigned count = LivingThings[Entity::DROP].size();
				for (unsigned i = 0; i < count; ++i)
				{
					if (LivingThings[Entity::DROP][i] == selectedEntity)
					{

					}
				}*/
				break;
			}
			default:
				break;
			}
		}
	}
	else if (bEButtonPressed && !Application::IsKeyPressed('E'))
		bEButtonPressed = false;

	if (selectedEntity)
	{
		switch (selectedEntity->entityID)
		{
		case Entity::HORSE:
		{
			tooltip = "[E] Mount"; break;
		}
		case Entity::DROP:
			tooltip = "[E] Loot"; break;
		}
	}

	if (player.mount)
		tooltip = "[E] Dismount";

	if (player.noClip)
		Update_LevelEditor(dt);
	else
		Update_Game(dt);

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

	elapsedTime += dt;

	static bool bGButtonPressed = false;

	if (!bGButtonPressed && Application::IsKeyPressed('G'))
	{
		Save("Save//save.txt"); Application::m_timer.getElapsedTime();
		bGButtonPressed = true;
	}
	else if (bGButtonPressed && !Application::IsKeyPressed('G'))
		bGButtonPressed = false;

	Application::ResetCursorPos();
}
Entity* MinScene::FetchEntity(unsigned id)
{
	switch (id)
	{
	case Entity::DROP:
	{
		unsigned count = LivingThings[Entity::DROP].size();
		for (unsigned i = 0; i < count; ++i)
		{
			if (!LivingThings[Entity::DROP][i]->IsActive())
			{
				LivingThings[Entity::DROP][i]->SetActive(true);
				LivingThings[Entity::DROP][i]->hOrientation = rand() % 360;
				return LivingThings[Entity::DROP][i];
			}
		}

		Drop* drop = new Drop();
		drop->hOrientation = rand() % 360;
		drop->entityID = Entity::DROP;
		drop->collision.hitbox.Set(0.35f, 0.04f, 0.35f);
		drop->collision.centre.Set(0, 0.02f, 0);

		LivingThings[Entity::DROP].push_back(drop);
		worldLivingThings[Entity::DROP].push_back(drop);
		return drop;
	}
	default:
		return NULL;
	}
	//if (id != 4)
	//{
	//	unsigned count = worldLivingThings[id].size();
	//	for (unsigned i = 0; i < count; ++i)
	//	{
	//		if (!worldLivingThings[id][i]->IsActive())
	//		{
	//			worldLivingThings[id][i]->SetActive(true);
	//			worldLivingThings[id][i]->lifetime = 0.f;
	//			worldLivingThings[id][i]->id = id;

	//			return worldLivingThings[id][i];
	//		}
	//	}
	//}

	//if (id == 4)
	//{
	//	Arrow* LivingThing = new Arrow();
	//	LivingThing->SetActive(true);
	//	LivingThing->id = id;
	//	worldLivingThings[id].push_back(LivingThing);
	//	LivingThings[id].push_back(LivingThing);
	//	return LivingThing;
	//}
	//else
	//{
	//	Living* LivingThing = new Living();
	//	LivingThing->SetActive(true);
	//	LivingThing->id = id;
	//	worldLivingThings[id].push_back(LivingThing);
	//	LivingThings[id].push_back(LivingThing);
	//	return LivingThing;
	//}
}
bool MinScene::GenerateArrow(Entity & source, float strength)
{
	Arrow* arrow = new Arrow();
	arrow->position = source.position; arrow->position.y += player.eyeLevel;
	arrow->velocity.SphericalToCartesian(source.hOrientation, source.vOrientation);
	arrow->velocity *= strength;
	arrow->hOrientation = source.hOrientation;
	arrow->vOrientation = source.vOrientation;
	arrow->climbHeight = 0.f;
	arrow->entityID = Entity::ARROW;
	arrow->collision.hitbox.Set(0.4f, 0.4f, 0.4f);
	arrow->collision.centre.Set(0, 0, 0);

	LivingThings[Entity::ARROW].push_back(arrow);
	worldLivingThings[Entity::ARROW].push_back(arrow);

	return true;
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

void MinScene::RenderMap()
{
	double x = 0;
	double y = 0;
	Application::GetCursorPos(&x, &y); y = Application::m_height - y;

	string text;

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.5f, 0);

	modelStack.PushMatrix();
	modelStack.Scale(Application::m_width, Application::m_height, 1);
	meshList["QUAD"]->textureID = NULL;  RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
	modelStack.Scale(1.06f, 6.f, 1);
	meshList["QUAD"]->textureID = textureID["MAP"];  RenderMesh(meshList["QUAD"], false, Color(0.68f,0.68f,0.68f));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Scale(Application::m_height, Application::m_height, 1);
	Color color(231.f/255.f, 206.f/255.f, 165.f/255.f);
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 1);
	glUniform3fv(m_parameters[U_COLOR_SCALE], 1, &color.r);
	glUniform1f(m_parameters[U_ALPHA], 0.7f);
	meshList["QUAD"]->textureID = m_lightDepthFBO.GetTexture();  RenderMesh(meshList["QUAD"], false);
	glUniform1f(m_parameters[U_ALPHA], 1);
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
	modelStack.PopMatrix();

	modelStack.PopMatrix();

	for (unsigned i = 0; i < waypointList.size(); ++i)
	{
		Vector3 thePoint = waypointList[i].getWaypoint(worldX, worldZ, Application::m_height, Application::m_height);

		modelStack.PushMatrix();
		modelStack.Translate(thePoint);
		modelStack.Scale(32);
		if (waypointList[i].selected)
		{
			modelStack.Scale(1.4f);
			text = waypointList[i].name;
			waypointList[i].selected = false;
		}
		meshList["QUAD"]->textureID = waypointList[i].texture;  RenderMesh(meshList["QUAD"], false, waypointList[i].color);
		modelStack.PopMatrix();
	}

	Vector3 thePoint = playerWaypoint.getWaypoint(worldX, worldZ, Application::m_height, Application::m_height);

	modelStack.PushMatrix();
	modelStack.Translate(thePoint);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(32);
	if (playerWaypoint.selected)
	{
		modelStack.Scale(1.4f);
		text = playerWaypoint.name;
		playerWaypoint.selected = false;
	}
	meshList["QUAD"]->textureID = playerWaypoint.texture;  RenderMesh(meshList["QUAD"], false, playerWaypoint.color);
	modelStack.PopMatrix();

	if (!text.empty())
	{
		float textWidth = Application::getTextWidth(text) * 20;

		glUniform1f(m_parameters[U_ALPHA], 0.75f);
		modelStack.PushMatrix();
		modelStack.Translate(16 + x + textWidth * 0.5f, y - 10, 0);
		modelStack.Scale(textWidth + 2, 22, 1);
		meshList["QUAD"]->textureID = NULL;  RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
		glUniform1f(m_parameters[U_ALPHA], 1);

		modelStack.PushMatrix();
		modelStack.Translate(16 + x, y - 10, 0);
		modelStack.Scale(20);
		RenderText(text, Color(1, 1, 1));
		modelStack.PopMatrix();
	}
}

void MinScene::Render()
{
	RenderPassGPass();
	RenderEntities_GPass();
	RenderBlocks_GPass();

	RenderPassMain();
	Mtx44 projection;
	modelStack.LoadIdentity();
	viewStack.LoadIdentity();
	viewStack.LookAt(camera->position.x, camera->position.y, camera->position.z, camera->target.x, camera->target.y, camera->target.z, camera->up.x, camera->up.y, camera->up.z);
	projection.SetToPerspective(camera->fov, 16.f / 9.f, 0.01f, 10000.f);
	projectionStack.LoadMatrix(projection);

	glUniformMatrix4fv(m_parameters[U_VIEW], 1, GL_FALSE, &viewStack.Top().a[0]);
	glUniformMatrix4fv(m_parameters[U_PROJECTION], 1, GL_FALSE, &projection.a[0]);

	Vector3 lightDir(0, 1, 0);
	Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);

	if (!showMap)
	{
		glUniform1i(m_parameters[U_FOG_ENABLED], 0);
		RenderSkybox();
		glUniform1i(m_parameters[U_FOG_ENABLED], 1);
		RenderScene();
		glUniform1i(m_parameters[U_FOG_ENABLED], 0);
	}

	glDisable(GL_DEPTH_TEST);

	viewStack.LoadIdentity();
	glUniformMatrix4fv(m_parameters[U_VIEW], 1, GL_FALSE, &viewStack.Top().a[0]);

	if (!showMap && !player.noClip)
	{
		if (player.getSelectedItem())
		{
			modelStack.PushMatrix();
			player.getSelectedItem()->RenderItem(modelStack);
			RenderMesh(player.getSelectedItem()->mesh, true);
			modelStack.PopMatrix();

			if (player.getSelectedItem()->itemID == CItem::BOW)
			{
				CBow* theBow = dynamic_cast<CBow*>(player.getSelectedItem());
				if (theBow->Charge > 0.3f)
				{
					modelStack.PushMatrix();
					theBow->RenderArrow(modelStack);
					RenderMesh(meshList["ARROW"], true);
					modelStack.PopMatrix();
				}
			}
		}
	}

	projection.SetToOrtho(0, Application::m_width, 0, Application::m_height, -80, 80);
	projectionStack.LoadMatrix(projection);
	glUniformMatrix4fv(m_parameters[U_PROJECTION], 1, GL_FALSE, &projection.a[0]);

	if (showMap)
		RenderMap();
	else
		Render2D();

	glEnable(GL_DEPTH_TEST);
}

void MinScene::RenderEntities_GPass()
{
	vector<Mtx44> MMat[6]; //Head, Body, Arm, Leg
	Mesh * mesh[6] = {
		meshList["HEAD"],
		meshList["BODY"],
		meshList["L_ARM"],
		meshList["L_LEG"],
		meshList["R_ARM"],
		meshList["R_LEG"] };
	vector<Mtx44> WolfMMat[4];
	Mesh * wolfMesh[4] = {
		meshList["WOLF_HEAD"],
		meshList["WOLF_BODY"],
		meshList["WOLF_LEG"],
		meshList["WOLF_TAIL"] };
	vector<Mtx44> HorseMMat;
	vector<Mtx44> DropMMat;

	for (unsigned j = 0; j < NumEntities; ++j)
	{
		if (j == Entity::ARROW)
			continue;

		unsigned count = LivingThings[j].size();

		for (unsigned i = 0; i < count; ++i)
		{
			if (LivingThings[j][i]->position.x >(int)camera->position.x + 47 || LivingThings[j][i]->position.x < (int)camera->position.x - 47 ||
				LivingThings[j][i]->position.z >(int)camera->position.z + 47 || LivingThings[j][i]->position.z < (int)camera->position.z - 47)
				continue;

			modelStack.PushMatrix();
			modelStack.Translate(LivingThings[j][i]->position);
			modelStack.Rotate(LivingThings[j][i]->hOrientation, 0, 1, 0);

			if (j == Entity::DROP)
			{
				DropMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}

			if (j == Entity::HORSE)
			{
				HorseMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}

			float rotation = LivingThings[j][i]->getSkeletalRotation();

			if (j == Entity::WOLF)
			{
				WolfMMat[0].push_back(modelStack.Top());
				WolfMMat[1].push_back(modelStack.Top());
				WolfMMat[2].push_back(modelStack.Top());
				WolfMMat[3].push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}

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
	}

	for (unsigned i = 0; i < 6; ++i)
	{
		if (!MMat[i].empty())
			RenderInstance(mesh[i], MMat[i].size(), &MMat[i][0], true);
	}

	for (unsigned i = 0; i < 4; ++i)
	{
		if (!WolfMMat[i].empty())
			RenderInstance(wolfMesh[i], WolfMMat[i].size(), &WolfMMat[i][0], true);
	}

	if (HorseMMat.size() > 0)
		RenderInstance(meshList["HORSE"], HorseMMat.size(), &HorseMMat[0], true);
	if (DropMMat.size() > 0)
		RenderInstance(meshList["MEAT"], DropMMat.size(), &DropMMat[0], true);
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
	vector<Mtx44> WolfMMat[4];
	Mesh * wolfMesh[4] = {
		meshList["WOLF_HEAD"],
		meshList["WOLF_BODY"],
		meshList["WOLF_LEG"],
		meshList["WOLF_TAIL"] };
	vector<Mtx44> HorseMMat;
	vector<Mtx44> ArrowMMat;
	vector<Mtx44> DropMMat;

	for (unsigned j = 0; j < NumEntities; ++j)
	{
		unsigned count = LivingThings[j].size();
		for (unsigned i = 0; i < count; ++i)
		{
			if (!Block(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox).collisionWithCylinder(camera->position + dir * RenderDist, RenderDist))
				continue;

			modelStack.PushMatrix();
			modelStack.Translate(LivingThings[j][i]->position);

			if (j != Entity::ARROW && j != Entity::DROP)
			{
				unsigned count2 = LivingThings[j][i]->StuckedArrows.size();
				for (unsigned k = 0; k < count2; ++k)
				{
					modelStack.PushMatrix();
					modelStack.Rotate(LivingThings[j][i]->hOrientation - LivingThings[j][i]->StuckedArrows[k]->relativeOrientation, 0, 1, 0);
					modelStack.Translate(LivingThings[j][i]->StuckedArrows[k]->relativePosition);
					modelStack.Rotate(LivingThings[j][i]->StuckedArrows[k]->hOrientation, 0, 1, 0);
					modelStack.Rotate(-LivingThings[j][i]->StuckedArrows[k]->vOrientation, 1, 0, 0);
					ArrowMMat.push_back(modelStack.Top());
					modelStack.PopMatrix();
				}
			}

			modelStack.Rotate(LivingThings[j][i]->hOrientation, 0, 1, 0);

			if (j == Entity::DROP)
			{
				DropMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}
			
			if (j == Entity::HORSE)
			{
				modelStack.Scale(LivingThings[j][i]->size);
				HorseMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}

			if (j == Entity::ARROW)
			{
				modelStack.Rotate(-LivingThings[j][i]->vOrientation, 1, 0, 0);
				ArrowMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}

			float rotation = LivingThings[j][i]->getSkeletalRotation();

			if (j == Entity::WOLF)
			{
				WolfMMat[1].push_back(modelStack.Top());

				modelStack.PushMatrix();
				modelStack.Translate(0, 0, 0.407f);
				modelStack.Rotate(LivingThings[j][i]->headOrientation, 0, 1, 0);
				modelStack.Translate(0, 0, -0.407f);
				WolfMMat[0].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 0.68125f, -0.561f);
				modelStack.Rotate(-rotation * 0.3f, 0, 1, 0);
				modelStack.Rotate(-10, 1, 0, 0);
				modelStack.Translate(0, -0.68125f, 0.561f);
				WolfMMat[3].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0.094f, 0, 0.188f);

				modelStack.PushMatrix();
				modelStack.Translate(0, 0.5f, 0);
				modelStack.Rotate(-rotation, 1, 0, 0);
				modelStack.Translate(0, -0.5f, 0);
				WolfMMat[2].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.Translate(0, 0, -0.44f - 0.188f);
				modelStack.Translate(0, 0.5f, 0);
				modelStack.Rotate(rotation, 1, 0, 0);
				modelStack.Translate(0, -0.5f, 0);
				WolfMMat[2].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-0.094f, 0, 0.188f);

				modelStack.PushMatrix();
				modelStack.Translate(0, 0.5f, 0);
				modelStack.Rotate(rotation, 1, 0, 0);
				modelStack.Translate(0, -0.5f, 0);
				WolfMMat[2].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.Translate(0, 0, -0.44f - 0.188f);
				modelStack.Translate(0, 0.5f, 0);
				modelStack.Rotate(-rotation, 1, 0, 0);
				modelStack.Translate(0, -0.5f, 0);
				WolfMMat[2].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.PopMatrix();

				continue;
			}

			MMat[j][0].push_back(modelStack.Top());
			MMat[j][1].push_back(modelStack.Top());

			modelStack.PushMatrix();
			modelStack.Translate(0, 1.5f, 0);
			modelStack.Rotate(rotation, 1, 0, 0);
			modelStack.Translate(0, -1.5f, 0);
			MMat[j][2].push_back(modelStack.Top());
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Translate(0, 0.75f, 0);
			modelStack.Rotate(rotation, 1, 0, 0);
			modelStack.Translate(0, -0.75f, 0);
			MMat[j][3].push_back(modelStack.Top());
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 1.5f, 0);
			modelStack.Rotate(-rotation, 1, 0, 0);
			modelStack.Translate(0, -1.5f, 0);
			MMat[j][4].push_back(modelStack.Top());
			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Translate(0, 0.75f, 0);
			modelStack.Rotate(-rotation, 1, 0, 0);
			modelStack.Translate(0, -0.75f, 0);
			MMat[j][5].push_back(modelStack.Top());
			modelStack.PopMatrix();

			modelStack.PopMatrix();
		}
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
	
	for (unsigned i = 0; i < 4; ++i)
	{
		wolfMesh[i]->textureID = textureID["WOLF"];

		if (!WolfMMat[i].empty())
			RenderInstance(wolfMesh[i], WolfMMat[i].size(), &WolfMMat[i][0], true);
	}

	if (HorseMMat.size() > 0)
		RenderInstance(meshList["HORSE"], HorseMMat.size(), &HorseMMat[0], true);
	if (ArrowMMat.size() > 0)
		RenderInstance(meshList["ARROW"], ArrowMMat.size(), &ArrowMMat[0], true);
	if (DropMMat.size() > 0)
		RenderInstance(meshList["MEAT"], DropMMat.size(), &DropMMat[0], true);
}

void MinScene::RenderBlocks_GPass()
{
	vector<Mtx44> MMat[2];

	if (!showMap)
	{
		unsigned count = blockList.size();
		for (unsigned i = 0; i < count; ++i)
		{
			if (blockList[i]->position.x >(int)camera->position.x + maxShadowCoord.x - 1 || blockList[i]->position.x < (int)camera->position.x + minShadowCoord.x ||
				blockList[i]->position.z >(int)camera->position.z + maxShadowCoord.z - 1 || blockList[i]->position.z < (int)camera->position.z + minShadowCoord.z)
				continue;

			modelStack.PushMatrix();
			blockList[i]->RenderObject(modelStack);
			if (blockList[i]->type == Block::STAIR)
				MMat[1].push_back(modelStack.Top());
			else
				MMat[0].push_back(modelStack.Top());
			modelStack.PopMatrix();
		}
	}
	else
	{
		for (int y = 0; y < worldY; ++y)
		{
			for (int x = 0; x < worldX; ++x)
			{
				for (int z = 0; z < worldZ; ++z)
				{
					if (worldBlockList[x][y][z])
					{
						modelStack.PushMatrix();
						worldBlockList[x][y][z]->RenderObject(modelStack);
						MMat[0].push_back(modelStack.Top());
						modelStack.PopMatrix();
					}
				}
			}
		}
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
		if (!blockList[i]->collisionWithCylinder(camera->position + dir * RenderDist, RenderDist))
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
		if (!alphaBlockList[i]->collisionWithCylinder(camera->position + dir * RenderDist, RenderDist))
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
	RenderEntities();
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
		vector<Mtx44> MMat;

		modelStack.PushMatrix();
		modelStack.Translate(player.initialPos.x - player.collision.hitbox.x * 0.5f, player.initialPos.y, player.initialPos.z - player.collision.hitbox.z * 0.5f);
		modelStack.Scale(player.collision.hitbox);
		MMat.push_back(modelStack.Top());
		modelStack.PopMatrix();

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

		for (unsigned j = 0; j < NumEntities; ++j)
		{
			count = LivingThings[j].size();
			for (unsigned i = 0; i < count; ++i)
			{
				modelStack.PushMatrix();
				modelStack.Translate(LivingThings[j][i]->position - LivingThings[j][i]->collision.hitbox * 0.5f);
				modelStack.Translate(LivingThings[j][i]->collision.centre);
				modelStack.Scale(LivingThings[j][i]->collision.hitbox);
				MMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
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
}

void MinScene::Render2D()
{
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
	ostringstream ss;

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.5f, 0);

	modelStack.PushMatrix();
	modelStack.Translate(0, -Application::m_height * 0.25f, 0);
	modelStack.Scale(24);
	modelStack.Translate(-Application::getTextWidth(tooltip) * 0.5f, 0, 0);
	RenderText(tooltip, Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f - 100, Application::m_height * 0.5f - 100, 0);

	modelStack.PushMatrix();
	modelStack.Rotate(-camera->orientation, 0, 0, 1);
	
	Color color(231.f / 255.f, 206.f / 255.f, 165.f / 255.f);
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 1);
	glUniform3fv(m_parameters[U_COLOR_SCALE], 1, &color.r);
	meshList["CIRCLE"]->textureID = m_lightDepthFBO.GetTexture();
	modelStack.PushMatrix();
	modelStack.Scale(180, 180, 1);
	RenderMesh(meshList["CIRCLE"], false);
	modelStack.PopMatrix();
	meshList["CIRCLE"]->textureID = NULL;
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(0, 80, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(20);
	modelStack.Translate(-Application::getTextWidth("N") * 0.5f, 0, 0);
	ss << "N";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(0, -80, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(20);
	modelStack.Translate(-Application::getTextWidth("S") * 0.5f, 0, 0);
	ss << "S";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(-80, 0, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(20);
	modelStack.Translate(-Application::getTextWidth("W") * 0.5f, 0, 0);
	ss << "W";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	ss.str("");
	modelStack.PushMatrix();
	modelStack.Translate(80, 0, 0);
	modelStack.Rotate(camera->orientation, 0, 0, 1);
	modelStack.Scale(20);
	modelStack.Translate(-Application::getTextWidth("E") * 0.5f, 0, 0);
	ss << "E";
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();
	
	for (unsigned i = 0; i < waypointList.size(); ++i)
	{
		Vector3 dir = waypointList[i].getWaypoint(96, 96, 180, 180) - playerWaypoint.getWaypoint(96, 96, 180, 180);

		if (dir.LengthSquared() >= 90*90)
			dir.Normalize() *= 90;

		modelStack.PushMatrix();
		modelStack.Translate(int(dir.x+0.5f), int(dir.y+0.5f),0);
		modelStack.Rotate(camera->orientation, 0, 0, 1);
		modelStack.Scale(24);
		meshList["QUAD"]->textureID = waypointList[i].texture;
		RenderMesh(meshList["QUAD"], false, waypointList[i].color);
		modelStack.PopMatrix();
	}

	modelStack.PopMatrix();
	modelStack.Scale(32);
	meshList["QUAD"]->textureID = playerWaypoint.texture;
	RenderMesh(meshList["QUAD"], false);
	modelStack.PopMatrix();
	meshList["QUAD"]->textureID = NULL;

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

	if (!player.noClip)
	{
		Mtx44 MMat[CInventory::InventorySize];

		for (unsigned i = 0; i < CInventory::InventorySize; ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate((Application::m_width - 64 * CInventory::InventorySize) + i * 64, 64, 0);
			modelStack.Scale(64);
			if (player.inventory.selectedSlot == player.inventory.slot[i])
				modelStack.Scale(1.5f);
			MMat[i] = modelStack.Top();
			modelStack.PopMatrix();
		}

		meshList["QUAD"]->textureID = textureID["SELECTOR"];
		RenderInstance(meshList["QUAD"], CInventory::InventorySize, &MMat[0], false);
		meshList["QUAD"]->textureID = NULL;
	}
	else
	{
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
	glUniform1i(m_parameters[U_TEXTURE_ROWS], 0);

	meshList["QUAD"]->textureID = textureID["SPIRITBAR"];
	modelStack.PushMatrix();
	modelStack.Translate(8 + 200 * 0.5f, 450, 0);
	modelStack.Scale(200, 16, 1);
	RenderMesh(meshList["QUAD"], false, Color(0.3f, 0.3f, 0.3f));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(8 + (ZoneBar/MaxZoneTime) * 200 * 0.5f, 450, 0);
	modelStack.Scale((ZoneBar / MaxZoneTime) * 200, 16, 1);
	RenderMesh(meshList["QUAD"], false, Color(1, 1, 1));
	modelStack.PopMatrix();

	color.Set(1,1,0);
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 1);
	glUniform3fv(m_parameters[U_COLOR_SCALE], 1, &color.r);

	meshList["QUAD"]->textureID = textureID["SPIRITBAR"];
	modelStack.PushMatrix();
	modelStack.Translate(8 + 200 * 0.5f, 350, 0);
	modelStack.Scale(200, 16, 1);
	RenderMesh(meshList["QUAD"], false, Color(0.3f, 0.3f, 0.3f));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(8 + (player.SprintBar/player.MaxSprintTime) * 200 * 0.5f, 350, 0);
	modelStack.Scale((player.SprintBar / player.MaxSprintTime) * 200, 16, 1);
	RenderMesh(meshList["QUAD"], false, Color(1, 1, 1));
	modelStack.PopMatrix();
	meshList["QUAD"]->textureID = NULL;
	
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
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

bool MinScene::LoadOutpost()
{
	unsigned count = 0;
	int lineCount = 1;
	string line;

	ifstream inputFile;
	inputFile.open("Save//Outpost.txt");

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
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);
				
				data[0] += 14;
 				data[2] -= 100;
				if (pos.x < 12 && pos.x > -40 && pos.z < 42 && pos.z > -13)
					if (FetchBlock(Vector3(data[0], data[1], data[2]), false, data[3], (Block::blockType)(int)data[4]))
						count++;
			}
			else
			{
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);

				data[0] += 14;
				data[2] -= 100;
				if (pos.x < 12 && pos.x > -40 && pos.z < 42 && pos.z > -13)
					if (FetchStair(Vector3(data[0], data[1], data[2]), false, data[3], data[5], data[6]))
						count++;
			}
			free(dup);
			data.clear();
			lineCount++;
		}

		inputFile.close();

		std::cout << "Loaded Outpost (" << count << " Blocks)" << endl;

		return true;
	}

	return false;
}


bool MinScene::LoadLionsDen()
{
	unsigned count = 0;
	int lineCount = 1;
	string line;

	ifstream inputFile;
	inputFile.open("Save//Lion.txt");

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
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);

				data[0] -= 25;
				data[2] += 68;
				if (pos.x < 35 && pos.x > -104 && pos.z < 60 && pos.z > -34)
					if (FetchBlock(Vector3(data[0], data[1], data[2]), false, data[3], (Block::blockType)(int)data[4]))
						count++;
			}
			else
			{
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);

				data[0] -= 25;
				data[2] += 68;
				if (pos.x < 35 && pos.x > -104 && pos.z < 60 && pos.z > -34)
					if (FetchStair(Vector3(data[0], data[1], data[2]), false, data[3], data[5], data[6]))
						count++;
			}
			free(dup);
			data.clear();
			lineCount++;
		}

		inputFile.close();

		std::cout << "Loaded Lion's Den (" << count << " Blocks)" << endl;

		return true;
	}

	return false;
}
bool MinScene::LoadCastleBlack()
{
	unsigned count = 0;
	int lineCount = 1;
	string line;

	ifstream inputFile;
	inputFile.open("Save//Castle Black.txt");

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
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);

				data[0] += 58;
				data[2] -= 26;
				if (pos.x < 70 && pos.x > -6 && pos.z < 72 && pos.z > -20)
					if (FetchBlock(Vector3(data[0], data[1], data[2]), false, data[3], (Block::blockType)(int)data[4]))
						count++;
			}
			else
			{
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);

				data[0] += 58;
				data[2] -= 26;
				if (pos.x < 70 && pos.x > -6 && pos.z < 72 && pos.z > -20)
					if (FetchStair(Vector3(data[0], data[1], data[2]), false, data[3], data[5], data[6]))
						count++;
			}
			free(dup);
			data.clear();
			lineCount++;
		}

		inputFile.close();

		std::cout << "Loaded Castle Black (" << count << " Blocks)" << endl;

		return true;
	}

	return false;
}
bool MinScene::LoadWinterfell()
{
	unsigned count = 0;
	int lineCount = 1;
	string line;

	ifstream inputFile;
	inputFile.open("Save//Winterfell.txt");

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
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);

				data[0] -= 81;
				if (pos.x < 3 && pos.x > -48 && pos.z < 28 && pos.z > -27)
					if (FetchBlock(Vector3(data[0], data[1], data[2]), false, data[3], (Block::blockType)(int)data[4]))
						count++;
			}
			else
			{
				Vector3 pos(data[0] - worldX*0.5f, data[1] - worldY * 0.5f, data[2] - worldZ * 0.5f);

				data[0] -= 81;
				if (pos.x < 3 && pos.x > -48 && pos.z < 28 && pos.z > -27)
					if (FetchStair(Vector3(data[0], data[1], data[2]), false, data[3], data[5], data[6]))
						count++;
			}
			free(dup);
			data.clear();
			lineCount++;
		}

		inputFile.close();

		std::cout << "Loaded Winterfell (" << count << " Blocks)" << endl;

		return true;
	}

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

	int y = 15;
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

					bool noSpaceForTree = false;

					for (unsigned i = 1; i <= 6; ++i)
					{
						if (noSpaceForTree)
							break;

						for (int X = x - 2; X <= x + 2; ++X)
						{
							if (noSpaceForTree)
								break;

							for (int Z = z - 2; Z <= z + 2; ++Z)
							{
								if (X < 0 || X >= worldX || Z < 0 || Z >= worldZ)
									continue;

								if (worldBlockList[X][y + i][Z])
								{
									noSpaceForTree = true;
									break;
								}
							}
						}
					}

					if (noSpaceForTree)
						continue;

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
	modelStack.Rotate(elapsedTime * 0.5f, 0, 180, 0);
	modelStack.Scale(500);

	modelStack.PushMatrix(); //UP
	modelStack.Translate(0, 0.4975f, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((0 % 3) / 3.f, 1 - ((0 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //DOWN
	modelStack.Translate(0, -0.4975f, 0);
	modelStack.Rotate(-90, 1, 0, 0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((1 % 3) / 3.f, 1 - ((1 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //LEFT
	modelStack.Translate(0.4975f, 0, 0);
	modelStack.Rotate(-90, 0, 1, 0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((2 % 3) / 3.f, 1 - ((2 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //RIGHT
	modelStack.Translate(-0.4975f,0,0);
	modelStack.Rotate(90,0,1,0);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((3 % 3) / 3.f, 1 - ((3 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //FRONT
	modelStack.Translate(0, 0, -0.4975f);
	MMat.push_back(modelStack.Top());
	offset.push_back(TexCoord((4 % 3) / 3.f, 1 - ((4 / 3) + 1) / 3.f));
	modelStack.PopMatrix();

	modelStack.PushMatrix(); //BACK
	modelStack.Translate(0, 0, 0.4975f);
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