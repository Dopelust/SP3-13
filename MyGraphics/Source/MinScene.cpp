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

void MinScene::InitGame()
{
	if (!player.inventory.slot[1]->item)
	{
		CBow* Bow = new CBow();
		Bow->mesh = meshList["BOW"];
		player.inventory.slot[1]->item = Bow;
	}
	if (!player.inventory.slot[0]->item)
	{
		CKnife* Knife = new CKnife();
		Knife->mesh = meshList["KNIFE"];
		player.inventory.slot[0]->item = Knife;
	}
	if (!player.inventory.slot[2]->item)
	{
		CFood* Food = new CFood();
		Food->mesh = meshList["MEAT"];
		player.inventory.slot[2]->item = Food;
	}
	player.inventory.slot[2]->item->count = 0;

	Load("Save//save1.txt");

	player.Init();
	camera = &player.camera;
	player.position.Set(108, -4, 28);
	player.hOrientation = 90;
	camera->position = player.position; camera->position.y += player.eyeLevel; camera->orientation = player.hOrientation;

	InitTextStuff();

	for (unsigned i = 0; i < 128; ++i)
	{
		Wolf* entity = new Wolf();
		entity->position.Set(rand() % 101 - 50, -7, rand() % 101 - 50);
		entity->hOrientation = entity->newOrientation = rand() % 360;
		entity->collision.hitbox.Set(0.7f, 0.8f, 0.7f);
		entity->collision.centre.Set(0, 0.4f, 0);
		entity->entityID = Entity::WOLF;
		entity->health = 50;
		worldLivingThings[Entity::WOLF].push_back(entity);
	}

	Key* entity = new Key();
	entity->position.Set(-18.5f, -7, 116.5f);
	entity->hOrientation = rand() % 360;
	entity->collision.hitbox.Set(0.3f, 0.9f, 0.3f);
	entity->collision.centre.Set(0, 0.45f, 0);
	entity->entityID = Entity::KEY;
	worldLivingThings[Entity::KEY].push_back(entity);

	entity = new Key();
	entity->position.Set(-105.5f, 17, 0.5f);
	entity->hOrientation = rand() % 360;
	entity->collision.hitbox.Set(0.3f, 0.9f, 0.3f);
	entity->collision.centre.Set(0, 0.45f, 0);
	entity->entityID = Entity::KEY;
	worldLivingThings[Entity::KEY].push_back(entity);

	entity = new Key();
	entity->position.Set(-14.5f, 2, -102.5f);
	entity->hOrientation = rand() % 360;
	entity->collision.hitbox.Set(0.3f, 0.9f, 0.3f);
	entity->collision.centre.Set(0, 0.45f, 0);
	entity->entityID = Entity::KEY;
	worldLivingThings[Entity::KEY].push_back(entity);

	entity = new Key();
	entity->position.Set(77.5f, -7, 38.5f);
	entity->hOrientation = rand() % 360;
	entity->collision.hitbox.Set(0.3f, 0.9f, 0.3f);
	entity->collision.centre.Set(0, 0.45f, 0);
	entity->entityID = Entity::KEY;
	worldLivingThings[Entity::KEY].push_back(entity);

	NPC* npc = new NPC();
	npc->position.Set(111.5f, -4, 27.5f);
	npc->hOrientation = -90;
	npc->collision.hitbox.Set(0.6f, 1.9f, 0.6f);
	npc->collision.centre.Set(0, 0.95f, 0);
	npc->entityID = Entity::NPC;
	npc->aggro = &player;
	npc->id = 1;
	worldLivingThings[Entity::NPC].push_back(npc);

	Entity* h = new Entity();
	h->position.Set(51,-8,5);
	h->hOrientation = -90;
	h->collision.hitbox.Set(1.4f, 2.1f, 1.4f);
	h->collision.centre.Set(0, 1.05f, 0);
	h->entityID = Entity::HORSE;
	worldLivingThings[Entity::HORSE].push_back(h);
	h = new Entity();
	h->position.Set(51, -8, 7);
	h->hOrientation = -90;
	h->collision.hitbox.Set(1.4f, 2.1f, 1.4f);
	h->collision.centre.Set(0, 1.05f, 0);
	h->entityID = Entity::HORSE;
	worldLivingThings[Entity::HORSE].push_back(h);
	h = new Entity();
	h->position.Set(51, -8, 9);
	h->hOrientation = -90;
	h->collision.hitbox.Set(1.4f, 2.1f, 1.4f);
	h->collision.centre.Set(0, 1.05f, 0);
	h->entityID = Entity::HORSE;
	worldLivingThings[Entity::HORSE].push_back(h);

	Knight* K = new Knight();
	K->position.Set(-18.5f, -8, 101.5f);
	K->health = 300;
	K->originalOrientation = K->hOrientation = 180;
	K->collision.hitbox.Set(0.6f, 1.9f, 0.6f);
	K->collision.centre.Set(0, 0.95f, 0);
	K->entityID = Entity::KNIGHT;
	K->id = 1;
	worldLivingThings[Entity::KNIGHT].push_back(K);

	UnleashTheHounds();

	InTheZone = false;
	ZoneBar = 0;
	MaxZoneTime = 12;

	keyCount = 0;
}

void MinScene::Init()
{
	Application::setWindowSize(1184, 666);
	SceneShadow::Init();
	Color fogColor(86.f / 255.f, 117.f / 255.f, 130.f / 255.f);
	glClearColor(86.f / 255.f, 117.f / 255.f, 130.f / 255.f, 0.0f);

	cout << "Initializing Light..." << endl;

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0,10,5);
	lights[0].power = 1.f;

	UpdateLight(1);

	cout << "Light Initialized." << endl << endl;

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

	//LoadOutpost();
	//LoadLionsDen();
	//LoadWinterfell();
	//LoadCastleBlack();
	//GenerateWorld(Vector3(worldX, 1, worldZ));

	InitGame();
	cout << worldBlocks <<  " Objects Initialized." << endl << endl;

	soundInit();

	minShadowCoord.Set(-48, -24, -48);
	maxShadowCoord.Set(48, 24, 48);
	showMap = false;

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

	Button mB;
	mB.position.Set(48, Application::m_height - 236, 0);
	mB.textScale = mB.min_textScale = 36; mB.max_textScale = 45;
	mB.name = "Play";
	mB.hitbox = mB.scale = Vector3(Application::getTextWidth(mB.name) * 36, 32, 1);
	mB.canSelect = true;
	menuButtons.push_back(mB);
	mB.position.Set(48, Application::m_height - 236 - 64, 0);
	mB.name = "Continue";
	mB.hitbox = mB.scale = Vector3(Application::getTextWidth(mB.name) * 36, 32, 1);
	mB.canSelect = false;
	menuButtons.push_back(mB);
	mB.position.Set(48, Application::m_height - 236 - 64 * 2, 0);
	mB.name = "Instructions";
	mB.hitbox = mB.scale = Vector3(Application::getTextWidth(mB.name) * 36, 32, 1);
	mB.canSelect = true;
	menuButtons.push_back(mB);
	mB.position.Set(48, Application::m_height - 236 - 64 * 3, 0);
	mB.name = "Credits";
	mB.hitbox = mB.scale = Vector3(Application::getTextWidth(mB.name) * 36, 32, 1);
	mB.canSelect = true;
	menuButtons.push_back(mB);
	mB.position.Set(48, Application::m_height - 236 - 64 * 4, 0);
	mB.name = "Exit";
	mB.hitbox = mB.scale = Vector3(Application::getTextWidth(mB.name) * 36, 32, 1);
	mB.canSelect = true;
	menuButtons.push_back(mB);
	mB.position.Set(Application::m_width * 0.5f, Application::m_height * 0.87f, 0);
	mB.name = "Key";
	mB.textScale = mB.min_textScale = 96; mB.max_textScale = 112;
	mB.hitbox = mB.scale = Vector3(96,96,96);
	mB.canSelect = true;
	menuButtons.push_back(mB);

	string AoW = "PoKeymon";
	float accumulatedSpacing = 0;
	mB.textScale = mB.min_textScale = 128; mB.max_textScale = 140;
	for (unsigned i = 0; i < AoW.size(); ++i)
	{
		mB.position.Set(32 + (accumulatedSpacing * 128), Application::m_height - 80, 0);
		mB.name = AoW[i];
		mB.hitbox = mB.scale = Vector3(Application::getTextWidth(mB.name) * 128, 100, 1);
		mB.canSelect = true;
		menuTitle.push_back(mB);

		accumulatedSpacing += Application::getTextWidth(mB.name);
	}

	menuCamera.Init(Vector3(0, -3, 0), 0, 0);
	camera = &menuCamera;
	GameState = MENU;
	QueuedState = NULL_STATE;
	Application::HideCursor(false);

	screenFade = 0;

	for (int x = -28; x <= 28; ++x)
	{
		for (int z = -28; z <= 28; ++z)
		{
			modelStack.PushMatrix();
			modelStack.Translate(x * 6, -8.25f, z * 6);
			modelStack.Rotate(-90, 1, 0, 0);
			modelStack.Scale(6);
			WaterMMat.push_back(modelStack.Top());
			modelStack.PopMatrix();
		}
	}

	string line;
	ifstream inputFile;
	inputFile.open("Assets//Text//Credits.txt");

	if (inputFile.is_open())
	{
		while (getline(inputFile, line))
		{
			credits.push_back(line);
		}
	}
	inputFile.close();

	inputFile.open("Assets//Text//Controls.txt");

	if (inputFile.is_open())
	{
		while (getline(inputFile, line))
		{
			instructions.push_back(line);
		}
	}
	inputFile.close();
}

void MinScene::UnleashTheHounds()
{
	for (unsigned z = 0; z < worldZ; ++z)
	{
		for (unsigned y = 0; y < worldY; ++y)
		{
			for (unsigned x = 0; x < worldX; ++x)
			{
				if (worldBlockList[x][y][z])
				{
					switch (worldBlockList[x][y][z]->id)
					{
					case 17:
					{
						Stair* s = dynamic_cast<Stair*>(worldBlockList[x][y][z]);
						Sentry* entity = new Sentry();
						entity->position = s->position + s->collision.centre;
						entity->hOrientation = s->orientation;
						entity->vOrientation = -30;
						entity->collision.hitbox.Set(0.6f, 1.9f, 0.6f);
						entity->collision.centre.Set(0, 0.95f, 0);
						entity->entityID = Entity::SENTRY;
						worldLivingThings[Entity::SENTRY].push_back(entity);

						RemoveBlock(worldBlockList[x][y][z]);
						break;
					}
					case 18:
					{
						Stair* s = dynamic_cast<Stair*>(worldBlockList[x][y][z]);
						Knight* entity = new Knight();
						entity->position = s->position + s->collision.centre;
						entity->originalOrientation = entity->hOrientation = s->orientation;
						entity->collision.hitbox.Set(0.6f, 1.9f, 0.6f);
						entity->collision.centre.Set(0, 0.95f, 0);
						entity->entityID = Entity::KNIGHT;
						worldLivingThings[Entity::KNIGHT].push_back(entity);

						RemoveBlock(worldBlockList[x][y][z]);
						break;
					}
					case 19:
					{
						Stair* s = dynamic_cast<Stair*>(worldBlockList[x][y][z]);
						NPC* entity = new NPC();
						entity->position = s->position + s->collision.centre;
						entity->hOrientation = s->orientation;
						entity->collision.hitbox.Set(0.6f, 1.9f, 0.6f);
						entity->collision.centre.Set(0, 0.95f, 0);
						entity->entityID = Entity::NPC;
						entity->aggro = &player;
						worldLivingThings[Entity::NPC].push_back(entity);

						RemoveBlock(worldBlockList[x][y][z]);
						break;
					}
					}
				}
			}
		}
	}
}
bool MinScene::FetchBlock(Vector3 pos, bool checkForCollision, unsigned blockID, Block::blockType type)
{
	if (pos.x >= worldX || pos.x < 0 || pos.y >= worldY || pos.y < 15 || pos.y >= worldY || pos.x < 0)
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
	if (pos.x >= worldX || pos.x < 0 || pos.y >= worldY || pos.y < 15 || pos.y >= worldY || pos.x < 0)
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
	Particle* p = FetchParticle(Particle::BLOCK);
	p->position = pos;
	p->velocity.Set(Math::RandFloatMinMax(-1.75f, 1.75f), rand() % 4 + 2, Math::RandFloatMinMax(-1.75f, 1.75f));
	p->size.Set(Math::RandFloatMinMax(0.05f, 0.15f), Math::RandFloatMinMax(0.05f, 0.15f), Math::RandFloatMinMax(0.05f, 0.15f));
	p->blockID = id;

	return true;
}

Particle* MinScene::FetchParticle(unsigned id)
{
	unsigned count = particleList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (!particleList[i]->IsActive() && particleList[i]->particleID == id)
		{
			particleList[i]->SetActive(true);
			particleList[i]->lifetime = 0;
			return particleList[i];
		}
	}

	if (id == Particle::ORB)
	{
		Orb* p = new Orb();
		particleList.push_back(p);
		return p;
	}

	Particle* p = new Particle();
	particleList.push_back(p);
	return p;
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
	int yBegin = Math::Max(15, (int)(index.y - RenderDist));
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
				for (int x = Position.x - 1; x <= Position.x + 1; ++x)
				{
					for (int z = Position.z - 1; z <= Position.z + 1; ++z)
					{
						for (int y = Position.y - 2; y <= Position.y + 2; ++y)
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
	for (int x = Position.x - 5; x <= Position.x + 5; ++x)
	{
		for (int y = Position.y - 5; y <= Position.y + 5; ++y)
		{
			for (int z = Position.z - 5; z <= Position.z + 5; ++z)
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

				view.Set(1 / view.x, 1 / view.y, 1 / view.z);

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
				GenerateArrow(&player, (player.inventory.selectedSlot->item->getCharge() - 0.4f) * 64);
				break;
			}
			case CItem::KNIFE:
			{
				if (selectedEntity && selectedEntity->IsLiving())
				{
					selectedEntity->aggro = &player;
					selectedEntity->Knockback((selectedEntity->position - camera->position) * 10 * player.damageCounter);

					for (unsigned l = 0; l < 5; ++l)
						SpawnParticle(selectedEntity->position + selectedEntity->collision.centre, 15);

					if (selectedEntity->entityID == Enemy::SENTRY || selectedEntity->entityID == Enemy::KNIGHT)
					{
						Vector3 playerDir; playerDir.SphericalToCartesian(player.hOrientation, 0);
						Vector3 enemyDir; enemyDir.SphericalToCartesian(selectedEntity->hOrientation, 0);

						if (playerDir.Dot(enemyDir) > 0.5f)
							selectedEntity->health = 0;
					}
				}
				break;
			}
			case CItem::FOOD:
			{
				if (player.health < player.maxHealth)
					player.health += 15;
				player.health = player.health > player.maxHealth ? player.maxHealth : player.health;

				player.getSelectedItem()->count--;
				break;
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
	static bool bESCButtonPressed = false;
	if (!bESCButtonPressed && Application::IsKeyPressed(VK_ESCAPE))
	{
		bESCButtonPressed = true;

		if (showMap)
		{
			showMap = false;
			Application::HideCursor(true);
		}
		else if (showStats)
		{
			showStats = false;
			Application::HideCursor(true);
		}
		else if (GameState == PLAY && !InTheZone)
			GameState = PAUSE;
		else
			bESCButtonPressed = false;
	}
	else if (bESCButtonPressed && !Application::IsKeyPressed(VK_ESCAPE))
		bESCButtonPressed = false;

	if (QueuedState)
	{
		Rise(screenFade, dt, 1);

		if (QueuedState == PLAY)
		{
			if (menuSound)
				menuSound->setVolume(1 - screenFade);
		}

		if (screenFade == 1 || QueuedState == VICTORY || QueuedState == DEFEAT)
		{
			switch (QueuedState)
			{
			case MENU:
				if (GameState == VICTORY || GameState == DEFEAT)
				{
					ExitGame();
					InitGame();
				}
				camera = &menuCamera;
				lights[0].position.Set(camera->position.x, 4, camera->position.z);
				Application::HideCursor(false);
				break;
			case PLAY:
				camera = &player.camera;
				Application::HideCursor(true);

				if (menuSound)
					engine->removeSoundSource(menuSound->getSoundSource());

				if (menuButtons[0].selected)
				{
					ExitGame();
					InitGame();
				}
				Application::ResetCursorPos();
				break;
			case CREDITS:
				creditsPosition = 0;
			default:
				break;
			}
			GameState = QueuedState;
			QueuedState = NULL_STATE;
		}
	}
	else
		Fall(screenFade, dt * 0.75f, 0);

	SceneBase::Update(dt);

	if (GameState == MENU)
	{
		if (!engine->isCurrentlyPlaying("Assets/Media/menu.mp3"))
			menuSound = engine->play2D("Assets/Media/menu.mp3", false, false, true);
	
		ObtainBlockList();
		PartitionCollision();

		menuCamera.orientation += dt * 0.25f;
		camera->Update();

		double x = 0;
		double y = 0;
		Application::GetCursorPos(&x, &y); y = Application::m_height - y;

		for (unsigned i = 0; i < menuTitle.size(); ++i)
		{
			Vector3 thePoint = menuTitle[i].position;
			if (menuTitle[i].canSelect && x < thePoint.x + menuTitle[i].hitbox.x && x > thePoint.x  && y > thePoint.y - menuTitle[i].hitbox.y * 0.5f && y < thePoint.y + menuTitle[i].hitbox.y * 0.5f)
			{
				menuTitle[i].selected = true;
				Rise(menuTitle[i].textScale, dt * 80, menuTitle[i].max_textScale);
			}
			else
			{
				menuTitle[i].selected = false;
				Fall(menuTitle[i].textScale, dt * 30, menuTitle[i].min_textScale);
			}
		}

		if (!QueuedState)
		{
			for (unsigned i = 0; i < menuButtons.size(); ++i)
			{
				Vector3 thePoint = menuButtons[i].position;
				if (menuButtons[i].canSelect && x < thePoint.x + menuButtons[i].hitbox.x && x > thePoint.x  && y > thePoint.y - menuButtons[i].hitbox.y * 0.5f && y < thePoint.y + menuButtons[i].hitbox.y * 0.5f)
				{
					menuButtons[i].selected = true;

					if (Application::IsMousePressed(0))
					{
						if (menuButtons[i].name == "Play" || menuButtons[i].name == "Continue")
							QueuedState = PLAY;
						else if (menuButtons[i].name == "Credits")
							QueuedState = CREDITS;
						else if (menuButtons[i].name == "Exit")
							Application::Close = true;
					}

				}
				else
					menuButtons[i].selected = false;
			}
		}

		for (unsigned i = 0; i < menuButtons.size(); ++i)
		{
			if (menuButtons[i].selected)
				Rise(menuButtons[i].textScale, dt * 25, menuButtons[i].max_textScale);
			else
				Fall(menuButtons[i].textScale, dt * 25, menuButtons[i].min_textScale);
		}

		Update_Entity(dt);
		return;
	}
	else if (GameState == CREDITS)
	{
		if (Application::IsKeyPressed(VK_SPACE))
			dt *= 8;

		creditsPosition += dt * 25;

		if (creditsPosition > 3400)
			QueuedState = MENU;

		if (!bESCButtonPressed && Application::IsKeyPressed(VK_ESCAPE))
		{
			QueuedState = MENU;
			bESCButtonPressed = true;
		}
		else if (bESCButtonPressed && !Application::IsKeyPressed(VK_ESCAPE))
			bESCButtonPressed = false;

		return;
	}
	else if (GameState == VICTORY || GameState == DEFEAT)
	{
		if (!bESCButtonPressed && Application::IsKeyPressed(VK_ESCAPE))
		{
			QueuedState = MENU;
			bESCButtonPressed = true;
			menuButtons[1].canSelect = false;
		}
		else if (bESCButtonPressed && !Application::IsKeyPressed(VK_ESCAPE))
			bESCButtonPressed = false;

		return;
	}
	else if (GameState == PAUSE)
	{
		if (!bESCButtonPressed && Application::IsKeyPressed(VK_ESCAPE))
		{
			Application::ResetCursorPos();
			GameState = PLAY;
			bESCButtonPressed = true;
		}
		else if (bESCButtonPressed && !Application::IsKeyPressed(VK_ESCAPE))
			bESCButtonPressed = false;

		if (Application::IsKeyPressed(VK_RETURN))
		{
			QueuedState = MENU;
			menuButtons[1].canSelect = true;
		}

		return;
	}

	if (!InTheZone)
	{
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
			else if (!showStats)
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

		static bool bPButtonPressed = false;

		if (!bPButtonPressed && Application::IsKeyPressed('P'))
		{
			bPButtonPressed = true;

			if (showStats)
			{
				Application::HideCursor(true);
				showStats = false;
			}
			else if (!showMap)
			{
				Application::HideCursor(false);
				Application::ResetCursorPos();
				showStats = true;
			}
		}
		else if (bPButtonPressed && !Application::IsKeyPressed('P'))
			bPButtonPressed = false;
	}

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
	else if (showStats)
	{
		double x = 0;
		double y = 0;
		Application::GetCursorPos(&x, &y);

		y = Application::m_height - y;

		static bool bLMouseButtonPressed = false;

		if (!bLMouseButtonPressed && Application::IsMousePressed(0))
		{
			if (x > Application::m_width * 0.65f && x < Application::m_width * 0.735f &&
				y < Application::m_height * 0.634f && y > Application::m_height * 0.51f)			//Top stat box
			{
				//DMG STAT GOES HERE
				player.addDamage = true;
			}
			else if (x > Application::m_width * 0.65f && x < Application::m_width * 0.735f &&
				y < Application::m_height * 0.46f && y > Application::m_height * 0.35f)
			{
				//MOVEMENT SPEED STAT GOES HERE
				player.addSpeed = true;
			}
			else if (x > Application::m_width * 0.65f && x < Application::m_width * 0.735f &&
				y < Application::m_height * 0.3f && y > Application::m_height * 0.19f)
			{
				//STAMINA STAT GOES HERE
				player.addStamina = true;
			}
			else if (x > Application::m_width * 0.65f && x < Application::m_width * 0.735f &&
				y < Application::m_height * 0.14f && y > Application::m_height * 0.03f)
			{
				//HEALTH STAT GOES HERE
				player.addHealth = true;
			}
			bLMouseButtonPressed = true;
		}
		else if (bLMouseButtonPressed && !Application::IsMousePressed(0))
		{
			bLMouseButtonPressed = false;
		}
		player.UpdateSkilltree(dt);
		return;
	}
	LocationTriggers(dt);

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
		glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);

	//static bool bNButtonPressed = false;

	//if (!bNButtonPressed && Application::IsKeyPressed(VK_TAB))
	//{
	//	if (!InTheZone)
	//		player.noClip = player.noClip ? false : true;
	//	bNButtonPressed = true;
	//}
	//else if (bNButtonPressed && !Application::IsKeyPressed(VK_TAB))
	//	bNButtonPressed = false;

	Update_Entity(dt);

	tooltip.clear();
	selectedEntity = NULL;
	selectedBlock = NULL;

	float dist = INT_MAX;
	Vector3 view = (camera->target - camera->position).Normalized();
	view.Set(1.0f / view.x, 1.0f / view.y, 1.0f / view.z);

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

	unsigned count = player.collisionBlockList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		float rayDist = player.collisionBlockList[i]->collisionWithRay(camera->position, view);
		if (rayDist >= 0 && rayDist <= 5)
		{
			if (rayDist < dist)
			{
				selectedBlock = player.collisionBlockList[i]; selectedEntity = NULL;
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
				player.inventory.slot[2]->item->count++;
				selectedEntity->SetActive(false);
				break;
			}
			case Entity::KEY:
			{
				selectedEntity->SetActive(false);

				for (unsigned i = 0; i < LivingThings[selectedEntity->entityID].size(); ++i)
				{
					if (LivingThings[selectedEntity->entityID][i] == selectedEntity)
						LivingThings[selectedEntity->entityID].erase(LivingThings[selectedEntity->entityID].begin() + i);
				}

				keyCount++;

				if (keyCount >= 4)
					QueuedState = VICTORY;
				
				break;
			}
			case Entity::NPC:
			{
				if (subtitle.empty())
				{
					subtitle = selectedEntity->getSpeech();
					subtitleTimer = 0;
				}
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
			tooltip = "[E] Mount"; break;
		case Entity::DROP:
			tooltip = "[E] Loot"; break;
		case Entity::KEY:
			tooltip = "[E] Take"; break;
		case Entity::NPC:
			tooltip = "[E] Talk"; break;
		}
	}

	if (player.mount)
		tooltip = "[E] Dismount";
	if (QueuedState == VICTORY)
		tooltip.clear();

	if (player.noClip)
		Update_LevelEditor(dt);
	else
		Update_Game(dt);

	count = particleList.size();
	for (int i = 0; i < count; ++i)
	{
		if (!particleList[i]->IsActive())
			continue;

		particleList[i]->Update(dt);

		if (particleList[i]->particleID == Particle::ORB)
		{
			if (particleList[i]->position.DistSquared(player.camera.position) < 0.1f*0.1f)
			{
				switch (particleList[i]->getSubID())
				{
				case Orb::HEALTH:
					player.health++;
					break;
				case Orb::EXPERIENCE:
				case Orb::ZONE:
					ZoneBar += dt * 10;
					ZoneBar = ZoneBar > MaxZoneTime ? MaxZoneTime : ZoneBar;
					break;
				default: break;
				}

				particleList[i]->SetActive(false);
			}
		}
	}

	elapsedTime += dt;
	subtitleTimer += dt;

	static bool bGButtonPressed = false;

	//if (!bGButtonPressed && Application::IsKeyPressed('G'))
	//{
	//	Save("Save//save1.txt"); Application::m_timer.getElapsedTime();
	//	bGButtonPressed = true;
	//}
	//else if (bGButtonPressed && !Application::IsKeyPressed('G'))
	//	bGButtonPressed = false;

	if (player.health <= 0)
		QueuedState = DEFEAT;

	Application::ResetCursorPos();
}

void MinScene::Update_Entity(double dt)
{
	Block Player(player.position, player.collision.centre, player.collision.hitbox);
	for (unsigned j = 0; j < NumEntities; ++j)
	{
		unsigned count = LivingThings[j].size();

		for (int i = 0; i < count; ++i)
		{
			if (LivingThings[j][i]->IsDead() || !LivingThings[j][i]->IsActive())
				continue;

			LivingThings[j][i]->Update(dt, false);

			if (j == Entity::ARROW) //If Arrows,
			{
				Arrow* theArrow = dynamic_cast<Arrow*>(LivingThings[j][i]);
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

				if (theArrow->source != &player && Block::checkCollision(arrow, Player)) //If arrow collide with entity
				{
					player.Knockback(LivingThings[j][i]->velocity); //Knockback entity
					for (unsigned l = 0; l < 10; ++l) //Generate Blood
						SpawnParticle(LivingThings[j][i]->position, 15);
					LivingThings[j][i]->SetActive(false);

					continue;
				}

				for (unsigned j2 = 0; j2 < NumEntities; ++j2) //Check with the other entities
				{
					if (j2 == Entity::ARROW || j2 == Entity::DROP || j2 == Entity::KEY) //Which are not arrows/drops
						continue;

					unsigned count2 = LivingThings[j2].size();

					for (unsigned k = 0; k < count2; ++k)
					{
						if (theArrow->source == LivingThings[j2][k])
							continue;

						Block NPC(LivingThings[j2][k]->position, LivingThings[j2][k]->collision.centre, LivingThings[j2][k]->collision.hitbox);

						if (Block::checkCollision(arrow, NPC)) //If arrow collide with entity
						{
							LivingThings[j2][k]->aggro = &player;
							LivingThings[j2][k]->Knockback(LivingThings[j][i]->velocity); //Knockback entity
							for (unsigned l = 0; l < 10; ++l) //Generate Blood
								SpawnParticle(LivingThings[j][i]->position, 15);

							theArrow->relativeOrientation = LivingThings[j2][k]->hOrientation;
							theArrow->relativePosition = LivingThings[j][i]->position - LivingThings[j2][k]->position;
							LivingThings[j2][k]->StuckedArrows.push_back(theArrow); //Add arrow to internal list
							LivingThings[j][i]->SetDead(true);

							unsigned count3 = worldLivingThings[j].size();
							for (unsigned l = 0; l < count3; ++l)
							{
								if (worldLivingThings[j][l] == LivingThings[j][i])
								{
									worldLivingThings[j].erase(worldLivingThings[j].begin() + l); //Remove from world list
									break;
								}
							}

							if (LivingThings[j2][k]->health <= 0)
							{
								//Add EXP and Zone
							}

							LivingThings[j].erase(LivingThings[j].begin() + i);
							--count;
							break;
						}
					}
				}
			}
			else if (LivingThings[j][i]->IsLiving())
			{
				Block NPC(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox);

				for (unsigned j2 = 0; j2 < NumEntities; ++j2) //Check with the other entities
				{
					if (j2 == Entity::ARROW || j2 == Entity::DROP || j2 == Entity::KEY)  //Which are not arrows/drops
						continue;

					unsigned count2 = LivingThings[j2].size();

					for (unsigned k = 0; k < count2; ++k)
					{
						if (LivingThings[j][i] == LivingThings[j2][k])
							continue;

						Block NPC2(LivingThings[j2][k]->position, LivingThings[j2][k]->collision.centre, LivingThings[j2][k]->collision.hitbox);

						if (Block::checkCollision(NPC2, NPC)) //If 2 entities collide
						{
							Vector3 NPCDir = LivingThings[j][i]->position - LivingThings[j2][k]->position;
							Vector3 NPC2Dir = LivingThings[j2][k]->position - LivingThings[j][i]->position;

							LivingThings[j][i]->kbVelocity += NPCDir * 0.5f;
							LivingThings[j2][k]->kbVelocity += NPC2Dir * 0.5f;
						}
					}
				}

				if (Block::checkCollision(Player, NPC)) //If 2 entities collide
				{
					Vector3 NPCDir = LivingThings[j][i]->position - player.position;
					Vector3 PlayerDir = player.position - LivingThings[j][i]->position;

					LivingThings[j][i]->kbVelocity += NPCDir;
					player.kbVelocity += PlayerDir * 0.5f;
				}

				if (j == Entity::SENTRY || j == Entity::KNIGHT)
				{
					if (!LivingThings[j][i]->aggro)
					{
						Vector3 position(LivingThings[j][i]->position.x, LivingThings[j][i]->position.y + 1.62f, LivingThings[j][i]->position.z);
						float dist = LivingThings[j][i]->viewRange = LivingThings[j][i]->maxViewRange;
						Vector3 view; view.SphericalToCartesian(LivingThings[j][i]->hOrientation + LivingThings[j][i]->headOrientation, LivingThings[j][i]->vOrientation);
						view.Set(1 / view.x, 1 / view.y, 1 / view.z);

						unsigned count2 = blockList.size();
						for (unsigned k = 0; k < count2; ++k)
						{
							if (position.DistSquared(blockList[k]->position) > LivingThings[j][i]->maxViewRange * LivingThings[j][i]->maxViewRange)
								continue;

							float rayDist = blockList[k]->collisionWithRay(position, view);
							if (rayDist >= 0 && rayDist <= LivingThings[j][i]->maxViewRange)
							{
								if (rayDist < dist)
								{
									dist = rayDist;
								}
							}
						}

						LivingThings[j][i]->viewRange = dist;

						float rayDist = Player.collisionWithRay(position, view);
						if (rayDist >= 0 && rayDist <= LivingThings[j][i]->maxViewRange)
							if (rayDist < dist)
							{
								LivingThings[j][i]->viewRange = dist;
								LivingThings[j][i]->aggro = &player;
							}
					}
					else if (j == Entity::SENTRY && LivingThings[j][i]->canAttack())
					{
						GenerateArrow(LivingThings[j][i], 35);
						LivingThings[j][i]->Attack();
					}
				}
			}
		}
	}
	for (unsigned j = 0; j < NumEntities; ++j) //Check For Deaths
	{
		if (j == Entity::DROP || j == Entity::ARROW)
			continue;

		unsigned count = LivingThings[j].size();

		for (int i = 0; i < count; ++i)
		{
			if (LivingThings[j][i]->health <= 0) //If the entity dies
			{
				player.updateEXP(dt);

				if (!InTheZone)
				{
					ZoneBar++;
					ZoneBar = ZoneBar > MaxZoneTime ? MaxZoneTime : ZoneBar;
				}

				for (unsigned l = 0; l < 10; ++l)
					SpawnParticle(LivingThings[j][i]->position, 15); //Generate more blood

				LivingThings[j][i]->ClearArrows();
				LivingThings[j][i]->SetActive(false);

				for (unsigned l = 0; l < 6; l++)
				{
					Entity* meat = FetchEntity(Entity::DROP);
					meat->position = LivingThings[j][i]->position;
					meat->velocity.Set(rand() % 21 - 10, rand() % 16 + 5, rand() % 21 - 10);
				}

				LivingThings[j].erase(LivingThings[j].begin() + i);
				--count;
				continue;
			}
		}
	}
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
bool MinScene::GenerateArrow(Entity* source, float strength)
{
	char* soundFileLocation[3] = { "Assets/Media/Weapons/shoot1.mp3", "Assets/Media/Weapons/shoot2.mp3" , "Assets/Media/Weapons/shoot3.mp3" };
	ISound* sound = engine->play3D(soundFileLocation[rand() % 3], vec3df(source->position.x, source->position.y + 1.62f, source->position.z), false, true);
	if (sound)
	{
		sound->setVolume(0.2f);
		sound->setIsPaused(false);
	}

	Arrow* arrow = new Arrow();
	arrow->position = source->position; arrow->position.y += player.eyeLevel; 
	arrow->velocity.SphericalToCartesian(source->hOrientation, source->vOrientation); arrow->position -= arrow->velocity;
	arrow->velocity *= strength;
	arrow->hOrientation = source->hOrientation;
	arrow->vOrientation = source->vOrientation;
	arrow->climbHeight = 0.f;
	arrow->entityID = Entity::ARROW;
	arrow->collision.hitbox.Set(0.4f, 0.4f, 0.4f);
	arrow->collision.centre.Set(0, 0, 0);
	arrow->source = source;

	LivingThings[Entity::ARROW].push_back(arrow);
	worldLivingThings[Entity::ARROW].push_back(arrow);

	return true;
}
bool MinScene::RemoveBlock(Block* block)
{
	for (unsigned i = 0; i < 30; ++i)
		SpawnParticle(block->position + block->collision.centre, block->id);

	Vector3 index = PositionToIndex(block->position);
	worldBlockList[(int)index.x][(int)index.y][(int)index.z] = NULL;

	unsigned count = blockList.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (blockList[i] == block)
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
			if (alphaBlockList[i] == block)
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

	if (GameState == MENU)
	{
		projection.SetToOrtho(0, Application::m_width, 0, Application::m_height, -80, 80);
		projectionStack.LoadMatrix(projection);
		glUniformMatrix4fv(m_parameters[U_PROJECTION], 1, GL_FALSE, &projection.a[0]);

		meshList["QUAD"]->textureID = textureID["MENU_BUTTON"];
		modelStack.PushMatrix();
		modelStack.Translate(0, Application::m_height, 0);
		modelStack.Scale(Application::m_width, 256, 1);
		modelStack.Translate(0.5f, -0.5f, 0);
		modelStack.Rotate(-90, 0, 0, 1);
		RenderMesh(meshList["QUAD"], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Scale(Application::m_width, 256, 1);
		modelStack.Translate(0.5f, 0.5f, 0);
		modelStack.Rotate(90, 0, 0, 1);
		RenderMesh(meshList["QUAD"], false);
		modelStack.PopMatrix();
		meshList["QUAD"]->textureID = NULL;

		meshList["QUAD"]->textureID = textureID["SPRITE_KEY"];
		for (unsigned i = 0; i < menuTitle.size(); ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate(-5, -5, 0);
			modelStack.Translate(menuTitle[i].position);
			modelStack.Scale(menuTitle[i].textScale);
			RenderText(menuTitle[i].name, Color(0, 0, 0));
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(menuTitle[i].position);
			modelStack.Scale(menuTitle[i].textScale);
			RenderText(menuTitle[i].name, Color(1,1,1));
			modelStack.PopMatrix();
		}

		for (unsigned i = 0; i < menuButtons.size(); ++i)
		{
			if (menuButtons[i].name != "Key")
			{
				modelStack.PushMatrix();
				modelStack.Translate(-2, -2, 0);
				modelStack.Translate(16, menuButtons[i].position.y, 0);
				modelStack.Scale(menuButtons[i].min_textScale);
				RenderText(">", Color(0, 0, 0));
				modelStack.PopMatrix();
			}

			modelStack.PushMatrix();
			modelStack.Translate(-2, -2, 0);
			modelStack.Translate(menuButtons[i].position);
			modelStack.Scale(menuButtons[i].textScale);
			if (menuButtons[i].name == "Key")
			{
				modelStack.Translate(0.5f, 0, 0);
				RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
			}
			else
				RenderText(menuButtons[i].name, Color(0,0,0));
			modelStack.PopMatrix();

			Color color;
			if (menuButtons[i].selected)
				color.Set(1, 1, 1);
			else if (menuButtons[i].canSelect)
				color.Set(0.6f, 0.6f, 0.6f);
			else
				color.Set(0.3f, 0.3f, 0.3f);

			if (menuButtons[i].name != "Key")
			{
				modelStack.PushMatrix();
				modelStack.Translate(16, menuButtons[i].position.y, 0);
				modelStack.Scale(menuButtons[i].min_textScale);
				RenderText(">", color);
				modelStack.PopMatrix();
			}

			modelStack.PushMatrix();
			modelStack.Translate(menuButtons[i].position);
			modelStack.Scale(menuButtons[i].textScale);
			if (menuButtons[i].name == "Key")
			{
				modelStack.Translate(0.5f, 0, 0);
				RenderMesh(meshList["QUAD"], false);
			}
			else
				RenderText(menuButtons[i].name, color);
			modelStack.PopMatrix();
		}
		
		glUniform1f(m_parameters[U_ALPHA], (menuButtons[2].textScale - menuButtons[2].min_textScale) / (menuButtons[2].max_textScale - menuButtons[2].min_textScale));
		for (int i = 0; i < instructions.size(); ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate(-2,-2, 0);
			modelStack.Translate(Application::m_width * 0.35f, Application::m_height * 0.67f + (-24 * i), 0);
			modelStack.Scale(24);
			RenderText(instructions[i], Color(0,0,0));

			modelStack.PopMatrix();
			modelStack.PushMatrix();
			modelStack.Translate(Application::m_width * 0.35f, Application::m_height * 0.67f + (-24 * i), 0);
			modelStack.Scale(24);
			RenderText(instructions[i], Color(1, 1, 1));
			modelStack.PopMatrix();
		}

		glUniform1f(m_parameters[U_ALPHA], screenFade);
		meshList["QUAD"]->textureID = NULL;
		modelStack.PushMatrix();
		modelStack.Scale(5000);
		RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
		glUniform1f(m_parameters[U_ALPHA], 1);

		glEnable(GL_DEPTH_TEST);
		return;
	}
	else if (GameState == CREDITS)
	{
		projection.SetToOrtho(0, Application::m_width, 0, Application::m_height, -80, 80);
		projectionStack.LoadMatrix(projection);
		glUniformMatrix4fv(m_parameters[U_PROJECTION], 1, GL_FALSE, &projection.a[0]);

		meshList["QUAD"]->textureID = NULL;
		modelStack.PushMatrix();
		modelStack.Scale(5000);
		RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(Application::m_width - 8, 8, 0);
		modelStack.Scale(12);
		modelStack.Translate(-Application::getTextWidth("[SPACE] : Fast Forward l [ESC] : Return To Main Menu"), 0, 0);
		RenderText("[SPACE] : Fast Forward l [ESC] : Return To Main Menu", Color(1, 1, 1));

		modelStack.PopMatrix();
		for (int i = 0; i < credits.size(); ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate(Application::m_width * 0.5f, (creditsPosition - 16) + (-64 * i), 0);
			modelStack.Scale(32);
			modelStack.Translate(-Application::getTextWidth(credits[i]) * 0.5f, 0, 0);
			RenderText(credits[i], Color(1, 1, 1));
			modelStack.PopMatrix();
		}

		glUniform1f(m_parameters[U_ALPHA], screenFade);
		meshList["QUAD"]->textureID = NULL;
		modelStack.PushMatrix();
		modelStack.Scale(5000);
		RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
		glUniform1f(m_parameters[U_ALPHA], 1);

		glEnable(GL_DEPTH_TEST);
		return;
	}
	else if (GameState == VICTORY || GameState == DEFEAT || GameState == PAUSE)
	{
		projection.SetToOrtho(0, Application::m_width, 0, Application::m_height, -80, 80);
		projectionStack.LoadMatrix(projection);
		glUniformMatrix4fv(m_parameters[U_PROJECTION], 1, GL_FALSE, &projection.a[0]);

		Render2D();
		glUniform1f(m_parameters[U_ALPHA], 0.5f);
		meshList["QUAD"]->textureID = NULL;
		modelStack.PushMatrix();
		modelStack.Scale(5000);
		RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
		glUniform1f(m_parameters[U_ALPHA], 1);

		string BigText;
		string flavorText;
		string instructText;

		if (GameState == VICTORY)
		{
			BigText = "You Win!";
			flavorText = "You collected all 4 Keys! You deserve a Cookie!";
			instructText = "[ESC] to return to Main Menu";
		}
		else if (GameState == PAUSE)
		{
			BigText = "Paused";
			flavorText = "[ESC] : Resume";
			instructText = "[ENTER] : Return To Main Menu";
		}
		else
		{
			BigText = "You Died!";
			flavorText = "You failed to collect all 4 Keys! You owe me a Cookie!";
			instructText = "[ESC] to return to Main Menu";
		}

		modelStack.PushMatrix();
		modelStack.Translate(-5, -5, 0);
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.75f, 0);
		modelStack.Scale(128);
		modelStack.Translate(-Application::getTextWidth(BigText) * 0.5f, 0, 0);
		RenderText(BigText, Color(0, 0, 0));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.75f, 0);
		modelStack.Scale(128);
		modelStack.Translate(-Application::getTextWidth(BigText) * 0.5f, 0, 0);
		RenderText(BigText, Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-3, -3, 0);
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.75f - 200, 0);
		modelStack.Scale(32);
		modelStack.Translate(-Application::getTextWidth(flavorText) * 0.5f, 0, 0);
		RenderText(flavorText, Color(0, 0, 0));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.75f - 200, 0);
		modelStack.Scale(32);
		modelStack.Translate(-Application::getTextWidth(flavorText) * 0.5f, 0, 0);
		RenderText(flavorText, Color(1, 1, 1));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-3, -3, 0);
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.75f - 200 - 32, 0);
		modelStack.Scale(32);
		modelStack.Translate(-Application::getTextWidth(instructText) * 0.5f, 0, 0);
		RenderText(instructText, Color(0, 0, 0));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.75f - 200 - 32, 0);
		modelStack.Scale(32);
		modelStack.Translate(-Application::getTextWidth(instructText) * 0.5f, 0, 0);
		RenderText(instructText, Color(1, 1, 1));
		modelStack.PopMatrix();

		glUniform1f(m_parameters[U_ALPHA], screenFade);
		meshList["QUAD"]->textureID = NULL;
		modelStack.PushMatrix();
		modelStack.Scale(5000);
		RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
		glUniform1f(m_parameters[U_ALPHA], 1);

		glEnable(GL_DEPTH_TEST);
		return;
	}

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
	else if (showStats)
		RenderStats();
	else
	{
		Render2D();
		if (showPopUp == true)
			RenderPopUpBox();
	}

	glUniform1f(m_parameters[U_ALPHA], screenFade);
	meshList["QUAD"]->textureID = NULL;
	modelStack.PushMatrix();
	modelStack.Scale(5000);
	RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));
	modelStack.PopMatrix();
	glUniform1f(m_parameters[U_ALPHA], 1);

	glEnable(GL_DEPTH_TEST);
}

void MinScene::RenderStats()
{
	double x = 0;
	double y = 0;
	Application::GetCursorPos(&x, &y); y = Application::m_height - y;

	string text;

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.5f, 0);

	modelStack.PushMatrix();
	modelStack.Scale(Application::m_width, Application::m_height, 1);
	meshList["QUAD"]->textureID = NULL;
	RenderMesh(meshList["QUAD"], false, Color(0, 0, 0));

	modelStack.Scale(1, 1, 1);
	meshList["QUAD"]->textureID = textureID["STATSMENU"];
	RenderMesh(meshList["QUAD"], false, Color(0.68f, 0.68f, 0.68f));
	modelStack.PopMatrix();

	modelStack.PopMatrix();


	ostringstream damageText, movespeedText, staminaText, healthText, pointsLeftText;

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.64f, Application::m_height * 0.745f, 0);
	modelStack.Scale(60);
	pointsLeftText << player.skillPoint;		//<---- change to points available
	RenderText(pointsLeftText.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.4f, Application::m_height * 0.59f, 0);
	modelStack.Scale(30);
	damageText << "Damage: " << player.damageCounter;
	RenderText(damageText.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.4f, Application::m_height * 0.43f, 0);
	modelStack.Scale(26, 28, 1);
	movespeedText << "Attacks/Second: " << setprecision(3) << player.speedCounter;
	RenderText(movespeedText.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.4f, Application::m_height * 0.27f, 0);
	modelStack.Scale(30);
	staminaText << "Stamina: " << player.MaxSprintTime;
	RenderText(staminaText.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.4f, Application::m_height * 0.11f, 0);
	modelStack.Scale(30);
	healthText << "Health: " << player.maxHealth;
	RenderText(healthText.str(), Color(1, 1, 1));
	modelStack.PopMatrix();
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
	vector<Mtx44> KeyMMat;

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

			if (j == Entity::KEY)
			{
				KeyMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}

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
			modelStack.Rotate(LivingThings[j][i]->headOrientation, 0, 1, 0);
			MMat[0].push_back(modelStack.Top());
			modelStack.PopMatrix();

			MMat[1].push_back(modelStack.Top());

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
	if (KeyMMat.size() > 0)
		RenderInstance(meshList["KEY"], KeyMMat.size(), &KeyMMat[0], true);
}

void MinScene::RenderEntities()
{
	Vector3 dir = (camera->target - camera->position).Normalized();

	unsigned textureIDs[5] = { textureID["PLAYER3"],textureID["PLAYER2"], textureID["PLAYER5"], textureID["PLAYER4"], textureID["PLAYER1"] };
	Mesh * mesh[6] = {
		meshList["HEAD"],
		meshList["BODY"],
		meshList["L_ARM"],
		meshList["L_LEG"],
		meshList["R_ARM"],
		meshList["R_LEG"] };
	vector<Mtx44> MMat[5][6]; //Head, Body, Arm, Leg
	vector<Mtx44> BowMMat;
	vector<Mtx44> RayMMat;
	vector<Mtx44> WolfMMat[4];
	Mesh * wolfMesh[4] = {
		meshList["WOLF_HEAD"],
		meshList["WOLF_BODY"],
		meshList["WOLF_LEG"],
		meshList["WOLF_TAIL"] };
	vector<Mtx44> HorseMMat;
	vector<Mtx44> ArrowMMat;
	vector<Mtx44> DropMMat;
	vector<Mtx44> KeyMMat;

	for (unsigned j = 0; j < NumEntities; ++j)
	{
		unsigned count = LivingThings[j].size();
		for (unsigned i = 0; i < count; ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate(LivingThings[j][i]->position);

			if (j == Entity::SENTRY || j == Entity::KNIGHT)
			{
				modelStack.PushMatrix();
				modelStack.Translate(0, 1.75f, 0);
				modelStack.Rotate(LivingThings[j][i]->headOrientation + LivingThings[j][i]->hOrientation, 0, 1, 0);
				modelStack.Rotate(-LivingThings[j][i]->vOrientation, 1, 0, 0);
				modelStack.Scale(LivingThings[j][i]->viewRange);
				RayMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
			}

			if (!Block(LivingThings[j][i]->position, LivingThings[j][i]->collision.centre, LivingThings[j][i]->collision.hitbox).collisionWithCylinder(camera->position + dir * RenderDist, RenderDist))
			{
				modelStack.PopMatrix();
				continue;
			}

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

			if (j == Entity::KEY)
			{
				KeyMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
				continue;
			}

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

			int index = j + LivingThings[j][i]->getSubID();
			if (j >= Entity::NPC)
				index++;

			modelStack.PushMatrix();
			modelStack.Rotate(LivingThings[j][i]->headOrientation, 0, 1, 0);
			if (LivingThings[j][i]->vOrientation != 0)
			{
				modelStack.Translate(0, 1.5f, 0);
				modelStack.Rotate(-LivingThings[j][i]->vOrientation, 1, 0, 0);
				modelStack.Translate(0, -1.5f, 0);
			}
			MMat[index][0].push_back(modelStack.Top());
			modelStack.PopMatrix();

			MMat[index][1].push_back(modelStack.Top());

			if (j == Entity::SENTRY)
			{
				modelStack.PushMatrix();
				modelStack.Translate(-0.25f, 1.375f, 0);
				modelStack.Rotate(-70, 1, 0, 0);
				modelStack.Translate(0.25f, -1.375f, 0);
				MMat[j][2].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0.25f, 1.375f, 0);
				modelStack.Rotate(-35, 0, 1, 0);
				modelStack.Rotate(-70, 1, 0, 0);
				modelStack.Translate(-0.25f, -1.375f, 0);
				MMat[j][4].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(-0.33f, 1.2f, 0.6f);
				modelStack.Rotate(180, 0, 1, 0);
				modelStack.Scale(0.35f);
				BowMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(0, 1.2f, 0.6f);
				modelStack.Rotate(-35, 0, 1, 0);
				modelStack.Rotate(90, 0, 0, 1);
				ArrowMMat.push_back(modelStack.Top());
				modelStack.PopMatrix();
			}
			else
			{
				modelStack.PushMatrix();
				modelStack.Translate(0, 1.5f, 0);

				modelStack.PushMatrix();
				modelStack.Rotate(rotation, 1, 0, 0);
				modelStack.Translate(0, -1.5f, 0);
				MMat[index][2].push_back(modelStack.Top());
				modelStack.PopMatrix();

				modelStack.Rotate(-rotation, 1, 0, 0);
				modelStack.Translate(0, -1.5f, 0);
				MMat[index][4].push_back(modelStack.Top());
				modelStack.PopMatrix();
			}

			modelStack.PushMatrix();
			modelStack.Translate(0, 0.75f, 0);

			modelStack.PushMatrix();
			modelStack.Rotate(rotation, 1, 0, 0);
			modelStack.Translate(0, -0.75f, 0);
			MMat[index][3].push_back(modelStack.Top());
			modelStack.PopMatrix();

			modelStack.Rotate(-rotation, 1, 0, 0);
			modelStack.Translate(0, -0.75f, 0);
			MMat[index][5].push_back(modelStack.Top());
			modelStack.PopMatrix();

			modelStack.PopMatrix();
		}
	}

	for (unsigned j = 0; j < 5; ++j)
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
	if (RayMMat.size() > 0)
		RenderInstance(meshList["RAY"], RayMMat.size(), &RayMMat[0], false, Color(1,0,0));
	if (BowMMat.size() > 0)
		RenderInstance(meshList["BOW"], BowMMat.size(), &BowMMat[0], true);
	if (KeyMMat.size() > 0)
		RenderInstance(meshList["KEY"], KeyMMat.size(), &KeyMMat[0], true);
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

	glUniform1f(m_parameters[U_ALPHA], 0.5f);
	if (WaterMMat.size() > 0)
		RenderInstance(meshList["WATER"], WaterMMat.size(), &WaterMMat[0]);
	glUniform1f(m_parameters[U_ALPHA], 1);
	

	if (selectedBlock)
	{
		modelStack.PushMatrix();
		selectedBlock->RenderObject(modelStack);
		RenderMesh(meshList["WIREBLOCK"], false, Color(0, 0, 0));
		modelStack.PopMatrix();
	}

	unsigned count = particleList.size();
	if (count > 0)
	{
		vector<Mtx44> MMat[2];
		vector<TexCoord> texOffset[2];

		vector<Mtx44> OrbMMat[Orb::NUM_ORBS];

		sort(particleList.begin(), particleList.end(), pIsMoreThan);
		for (int i = 0; i < count; ++i)
		{
			if (!particleList[i]->IsActive())
				continue;

			modelStack.PushMatrix();
			modelStack.Translate(particleList[i]->position);
			modelStack.Rotate(camera->orientation + 180, 0, 1, 0);
			modelStack.Rotate(camera->look, 1, 0, 0);
			modelStack.Scale(particleList[i]->size);
			if (particleList[i]->particleID == Particle::BLOCK)
			{
				if (particleList[i]->blockID > 3)
				{
					texOffset[0].push_back(Block::getTextureOffset(particleList[i]->blockID));
					MMat[0].push_back(modelStack.Top());
				}
				else
				{
					texOffset[1].push_back(Block::getTextureOffset(particleList[i]->blockID, Block::MT));
					MMat[1].push_back(modelStack.Top());
				}
			}
			else if (particleList[i]->particleID == Particle::ORB)
				OrbMMat[particleList[i]->getSubID()].push_back(modelStack.Top());
			modelStack.PopMatrix();
		}

		glUniform1i(m_parameters[U_PARTICLE_TEXTURE], 1);
	
		if (MMat[0].size() > 0)
		{
			glUniform1i(m_parameters[U_TEXTURE_ROWS], 4);
			meshList["QUAD"]->textureID = meshList["BLOCK"]->textureID;
			RenderInstanceAtlas(meshList["QUAD"], MMat[0].size(), &MMat[0][0], &texOffset[0][0], false, Color(0.75f, 0.75f, 0.75f));
		}
		if (MMat[1].size() > 0)
		{
			glUniform1i(m_parameters[U_TEXTURE_ROWS], 6);
			meshList["QUAD"]->textureID = meshList["MT_BLOCK"]->textureID;
			RenderInstanceAtlas(meshList["QUAD"], MMat[1].size(), &MMat[1][0], &texOffset[1][0], false, Color(0.75f, 0.75f, 0.75f));
		}

		glUniform1i(m_parameters[U_PARTICLE_TEXTURE], 0);
		glUniform1i(m_parameters[U_TEXTURE_ROWS], 0);

		glDepthMask(FALSE);
		meshList["QUAD"]->textureID = textureID["ORB"];
		if (OrbMMat[Orb::HEALTH].size() > 0)
			RenderInstance(meshList["QUAD"], OrbMMat[Orb::HEALTH].size(), &OrbMMat[Orb::HEALTH][0], false, Color(0.8f,0,0));
		if (OrbMMat[Orb::EXPERIENCE].size() > 0)
			RenderInstance(meshList["QUAD"], OrbMMat[Orb::EXPERIENCE].size(), &OrbMMat[Orb::EXPERIENCE][0], false, Color(1, 1, 0));
		if (OrbMMat[Orb::ZONE].size() > 0)
			RenderInstance(meshList["QUAD"], OrbMMat[Orb::ZONE].size(), &OrbMMat[Orb::ZONE][0], false, Color(0, 0, 0.8f));

		meshList["QUAD"]->textureID = NULL;
		glDepthMask(GL_TRUE);
	}
}

void MinScene::Render2D()
{
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
	ostringstream ss;

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.5f, 0);

	modelStack.PushMatrix();
	modelStack.Translate(-2, -2, 0);
	modelStack.Translate(0, -Application::m_height * 0.25f, 0);
	modelStack.Scale(24);
	modelStack.Translate(-Application::getTextWidth(tooltip) * 0.5f, 0, 0);
	RenderText(tooltip, Color(0,0,0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0, -Application::m_height * 0.25f, 0);
	modelStack.Scale(24);
	modelStack.Translate(-Application::getTextWidth(tooltip) * 0.5f, 0, 0);
	RenderText(tooltip, Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(Application::m_width * 0.5f - 100, Application::m_height * 0.5f - 100, 0);

	modelStack.PushMatrix();
	modelStack.Translate(0, -180, 0);

	//LAZ SLOW MO
	meshList["QUAD"]->textureID = textureID["SPIRITBAR"];
	modelStack.PushMatrix();
	modelStack.Translate(-70 + 150 * 0.5f, 0, 0);
	modelStack.Scale(150, 12, 1);
	RenderMesh(meshList["QUAD"], false, Color(0.3f, 0.3f, 0.3f));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-70 + (ZoneBar / MaxZoneTime) * 150 * 0.5f, 0, 0);
	modelStack.Scale((ZoneBar / MaxZoneTime) * 150, 12, 1);
	RenderMesh(meshList["QUAD"], false, Color(1, 1, 1));
	modelStack.PopMatrix();

	meshList["QUAD"]->textureID = textureID["SPRITE_HOURGLASS"];
	modelStack.PushMatrix();
	modelStack.Translate(-72, 0, 0);
	modelStack.Scale(20);
	RenderMesh(meshList["QUAD"], false);
	modelStack.PopMatrix();

	if (ZoneBar == MaxZoneTime)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-2, -2, 0);
		modelStack.Scale(24);
		modelStack.Translate(-Application::getTextWidth("[L]") * 0.5f, 0, 0);
		RenderText("[L]", Color(0, 0, 0));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Scale(24);
		modelStack.Translate(-Application::getTextWidth("[L]") * 0.5f, 0, 0);
		RenderText("[L]", Color(1, 1, 1));
		modelStack.PopMatrix();
	}

	modelStack.PopMatrix();

	meshList["QUAD"]->textureID = textureID["SPRITE_KEY"];
	modelStack.PushMatrix();
	modelStack.Translate(-12, -128, 0);

	modelStack.PushMatrix();
	modelStack.Translate(0, -1, 0);
	modelStack.Scale(21);
	RenderMesh(meshList["QUAD"], false);
	modelStack.PopMatrix();

	ss.str("");
	ss << "x" << keyCount;

	modelStack.PushMatrix();
	modelStack.Translate(-2, -2, 0);
	modelStack.Translate(16, 0, 0);
	modelStack.Scale(24);
	RenderText(ss.str() , Color(0, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(16, 0, 0);
	modelStack.Scale(24);
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	modelStack.PopMatrix();
	meshList["QUAD"]->textureID = NULL;

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

	//modelStack.PushMatrix();
	//modelStack.Translate(6, 540, 0);
	//modelStack.Scale(20);
	//ss.str("");
	//ss.precision(5);
	//ss << (int)(fps + 0.5f) << " fps (" << (int)(minFPS + 0.5f) << "/" << (int)(peakFPS + 0.5f) << ") (" << blockList.size() << "/" << worldBlocks << " blocks)";
	//RenderText(ss.str(), Color(1, 1, 1));
	//modelStack.PopMatrix();

	if (!player.noClip)
	{
		Mtx44 MMat[CInventory::InventorySize];
		unsigned count = 0;

		for (unsigned i = 0; i < CInventory::InventorySize; ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate((Application::m_width - 64 * CInventory::InventorySize) + i * 64, 64, 0);

			if (player.inventory.selectedSlot == player.inventory.slot[i])
			{
				modelStack.Scale(96);
				MMat[CInventory::InventorySize - 1] = modelStack.Top();
			}
			else
			{
				modelStack.Scale(64);
				MMat[count] = modelStack.Top();
				++count;
			}
			modelStack.PopMatrix();
		}

		//Laz
		meshList["QUAD"]->textureID = textureID["SELECTOR"];
		RenderInstance(meshList["QUAD"], CInventory::InventorySize, &MMat[0], false);
		meshList["QUAD"]->textureID = NULL;

		for (unsigned i = 0; i < CInventory::InventorySize; ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate((Application::m_width - 64 * CInventory::InventorySize) + i * 64, 64, 0);

			modelStack.PushMatrix();
			if (player.inventory.selectedSlot == player.inventory.slot[i])
				modelStack.Scale(1.5f);
			switch (player.inventory.slot[i]->item->itemID)
			{
			case CItem::BOW:
				modelStack.Scale(36);
				meshList["QUAD"]->textureID = textureID["SPRITE_BOW"];
				RenderMesh(meshList["QUAD"], false); break;
			case CItem::KNIFE:
				modelStack.Scale(42);
				meshList["QUAD"]->textureID = textureID["SPRITE_KNIFE"];
				RenderMesh(meshList["QUAD"], false); break;
			case CItem::FOOD:
				modelStack.Scale(45);
				meshList["QUAD"]->textureID = textureID["SPRITE_FOOD"];
				RenderMesh(meshList["QUAD"], false);  break;
			}
			modelStack.PopMatrix();
			if (!player.inventory.slot[i]->item->unique)
			{
				ss.str("");
				ss << player.inventory.slot[i]->item->count; 

				modelStack.PushMatrix();
				modelStack.Translate(10, -10, 0);
				modelStack.Translate(-2, -2, 0);
				modelStack.Scale(24);
				RenderText(ss.str(), Color(0, 0, 0));
				modelStack.PopMatrix();

				modelStack.PushMatrix();
				modelStack.Translate(10, -10, 0);
				modelStack.Scale(24);
				RenderText(ss.str(), Color(1, 1, 1));
				modelStack.PopMatrix();
			}
			modelStack.PopMatrix();
		}
	}
	else
	{
		vector<Mtx44>MMat[Block::NUM_TYPES + 1];
		vector<TexCoord>texOffset[Block::NUM_TYPES];

		for (unsigned i = 0; i < blockInventory.size; ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate(Application::m_width * (i * 0.025f + 0.02f), Application::m_height * 0.05f, 0);

			modelStack.PushMatrix();
			modelStack.Scale(20);
			MMat[Block::NUM_TYPES].push_back(modelStack.Top());
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Rotate(25, 1, 0, 0);
			modelStack.Rotate(45, 0, 1, 0);
			if (&blockInventory.getBlock() == &blockInventory.getBlock(i))
				modelStack.Scale(1.5f);
			modelStack.Scale(16);
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

		if(!player.noClip)
	{
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 1);

	// LAZ HP BAR

	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
	meshList["QUAD"]->textureID = textureID["SPRITE_HEART"];
	modelStack.PushMatrix();
	modelStack.Translate(30, 72, 0);
	modelStack.Scale(32);
	RenderMesh(meshList["QUAD"], false);
	modelStack.PopMatrix();
	meshList["QUAD"]->textureID = NULL;

	//LAZ HP Counter
	ss.str("");
	ss << (int)player.health;
	modelStack.PushMatrix();
	modelStack.Translate(50 - 2, 70 - 2, 0);
	modelStack.Scale(24);
	RenderText(ss.str(), Color(0,0,0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(50, 70, 0);
	modelStack.Scale(24);
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 1);
	color.Set(1,1,0);
	glUniform3fv(m_parameters[U_COLOR_SCALE], 1, &color.r);

	//LAZ SPRINT BAR
	meshList["QUAD"]->textureID = textureID["SPIRITBAR"];
	modelStack.PushMatrix();
	modelStack.Translate(100 + 200 * 0.5f, 70, 0);
	modelStack.Scale(200, 16, 1);
	RenderMesh(meshList["QUAD"], false, Color(0.3f, 0.3f, 0.3f));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(100 + (player.SprintBar/player.MaxSprintTime) * 200 * 0.5f, 70, 0);
	modelStack.Scale((player.SprintBar / player.MaxSprintTime) * 200, 16, 1);
	RenderMesh(meshList["QUAD"], false, Color(1, 1, 1));
	modelStack.PopMatrix();

	//LAZ EXP BAR
	modelStack.PushMatrix();
	modelStack.Scale(Application::m_width, 16, 1);
	modelStack.Translate(0.5f, 0, 0);
	RenderMesh(meshList["QUAD"], false, Color(0.2f, 0.2f, 0.2f));
	modelStack.PopMatrix();

	float scale = (player.currentEXP / player.maxEXP) * Application::m_width;
	modelStack.PushMatrix();
	modelStack.Scale(scale, 16, 1);
	modelStack.Translate(0.5f, 0, 0);
	RenderMesh(meshList["QUAD"], false, Color(1, 1, 1));
	modelStack.PopMatrix();	

	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
	if (player.skillPoint > 0)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-2, -2, 0);
		modelStack.Translate(Application::m_width * 0.5f, 12, 0);
		modelStack.Scale(24);
		modelStack.Translate(-Application::getTextWidth("[P] : Skill Point Available!") * 0.5f, 0, 0);
		RenderText("[P] : Skill Point Available!", Color(0, 0, 0));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(Application::m_width * 0.5f, 12, 0);
		modelStack.Scale(24);
		modelStack.Translate(-Application::getTextWidth("[P] : Skill Point Available!") * 0.5f, 0, 0);
		RenderText("[P] : Skill Point Available!", Color(1, 1, 1));
		modelStack.PopMatrix();
	}

	ss.str("");
	ss << "Level " << player.Lv;

	modelStack.PushMatrix();
	modelStack.Translate(-3, -3, 0);
	modelStack.Translate(4, 16, 0);
	modelStack.Scale(32);
	RenderText(ss.str(), Color(0, 0, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(4, 16, 0);
	modelStack.Scale(32);
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	//LAZ SPRINT COUNTER
	modelStack.PushMatrix();
	modelStack.Translate(180, 70, 0);
	modelStack.Scale(20);
	ss.str("");
	ss.precision(3);
	ss << player.SprintCounter<< "%"; 
	RenderText(ss.str(), Color(1, 1, 1));
	modelStack.PopMatrix();

	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);

	//Sprites
	meshList["QUAD"]->textureID = textureID["SPRITE_SPRINT"];
	modelStack.PushMatrix();
	modelStack.Translate(99 , 70,0);
	modelStack.Scale(24);
	RenderMesh(meshList["QUAD"],false);
	modelStack.PopMatrix();
	meshList["QUAD"]->textureID = NULL;
	glUniform1i(m_parameters[U_COLOR_SCALE_ENABLED], 0);
	}

	if (!subtitle.empty())
	{
		float a = 1;
		if (subtitleTimer > 4)
			a = 5 - subtitleTimer;
		if (subtitleTimer > 5)
		{
			a = 0;
			subtitle.clear();
		}

		glUniform1f(m_parameters[U_ALPHA], a);
		modelStack.PushMatrix();
		modelStack.Translate(-2, -2, 0);
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.2f, 0);
		modelStack.Scale(24);
		modelStack.Translate(-Application::getTextWidth(subtitle) * 0.5f, 0, 0);
		RenderText(subtitle, Color(0, 0, 0));
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.2f, 0);
		modelStack.Scale(24);
		modelStack.Translate(-Application::getTextWidth(subtitle) * 0.5f, 0, 0);
		RenderText(subtitle, Color(1, 1, 1));
		modelStack.PopMatrix();
		glUniform1f(m_parameters[U_ALPHA], 1);
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
				if (worldBlockList[x][(int)terrainRise[i].y - 1][z] && worldBlockList[x][(int)terrainRise[i].y - 1][z]->id == 0)
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
	return;
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

void MinScene::ExitGame()
{
	while (particleList.size() > 0)
	{
		delete particleList.back();
		particleList.pop_back();
	}

	for (unsigned j = 0; j < NumEntities; ++j)
	{
		while (worldLivingThings[j].size() > 0)
		{
			delete worldLivingThings[j].back();
			worldLivingThings[j].pop_back();
		}

		LivingThings[j].clear();
	}
}

void MinScene::Exit()
{
	soundExit();
	ExitGame();

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