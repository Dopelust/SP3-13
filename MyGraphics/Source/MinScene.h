/******************************************************************************/
/*!
\file	MinScene.h
\author SP2 Team 16
\par	
\brief
Header file for the Project/SP Scene.
*/
/******************************************************************************/
#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H

#include "Minimap.h"
#include "Frustum.h"
#include "Living.h"
#include "SceneShadow.h"
#include "Bow.h"
#include "Knife.h"
#include <map>

#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;

#define worldX 256
#define worldY 48
#define worldZ 256

class MinScene : public SceneShadow
{
public:
	MinScene();
	~MinScene();

	void Init();
	void Update(double dt);
	void Update_LevelEditor(double dt);
	void Update_Game(double dt);

	Entity* FetchEntity(unsigned id);
	bool GenerateArrow(Entity& source, float strength);
	bool RemoveBlock(Block* block);
	bool FetchBlock(Vector3 pos, bool checkForCollision = false, unsigned blockID = rand() % NumBlocks, Block::blockType type = Block::DEFAULT);
	bool FetchStair(Vector3 pos, bool checkForCollision = false, unsigned blockID = rand() % (NumBlocks - 3) + 3, int orientation = 0, int look = 0);
	bool SpawnParticle(Vector3 pos, unsigned id);
	void UpdateInput(const unsigned char key);

	void Render();
	void Exit();

	int soundInit();
	int soundUpdate(Player player);
	int soundExit();

	static Vector3 PositionToIndex(Vector3 pos);

private:
	vector<Entity*> LivingThings[NumEntities];
	vector<Entity*> worldLivingThings[NumEntities];

	Block* selectedBlock;
	Entity* selectedEntity;

	unsigned worldBlocks;
	Block* worldBlockList[worldX][worldY][worldZ];
	vector<Block*> blockList;
	vector<Block*> alphaBlockList;
	vector<Particle> particleList;
	void ObtainBlockList();
	void PartitionCollision();
	BlockFactory blockInventory;

	void RenderBlocks();
	void RenderBlocks_GPass();
	void RenderEntities();
	void RenderEntities_GPass();
	void RenderScene();
	void RenderSkybox();
	void Render2D();
	void RenderMap();

	unsigned nextUpdate;
	float RenderDist;
	Player player;

	string tooltip;

	bool Save(const char* filepath);
	bool Load(const char* filepath);

	bool LoadWinterfell();
	bool LoadOutpost();
	bool LoadLionsDen();
	bool LoadCastleBlack();

	bool GenerateWorld(Vector3 size);

	bool Convert(const char* filepath);

	float ZoneBar;
	float MaxZoneTime;
	bool InTheZone;

	bool showMap;
	Waypoint playerWaypoint;
	vector<Waypoint> waypointList;
};

#endif
