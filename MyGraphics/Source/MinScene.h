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

#include "Frustum.h"
#include "Living.h"
#include "SceneShadow.h"
#include <map>

#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;

#define worldX 256
#define worldY 32
#define worldZ 256

class MinScene : public SceneShadow
{
public:
	static const int GRAVITY = -98;
	static const int MAX_VELOCITY = 21;

	MinScene();
	~MinScene();

	void Init();
	void Update(double dt);
	bool RemoveBlock(Block* block);
	bool FetchBlock(Vector3 pos, bool checkForCollision = false, unsigned blockID = rand() % NumBlocks, Block::blockType type = Block::DEFAULT);
	bool FetchStair(Vector3 pos, bool checkForCollision = false, unsigned blockID = rand() % (NumBlocks - 3) + 3, int orientation = 0, int look = 0);
	bool SpawnParticle(Vector3 pos, unsigned id);
	void UpdateInput (const unsigned char key);

	void Render();
	void Exit();

	int soundInit();
	int soundUpdate(Player player);
	int soundExit();

	static Vector3 PositionToIndex(Vector3 pos);

private:
	vector<Living*> LivingThings;

	unsigned worldBlocks;
	Block* selectedBlock;
	Block* worldBlockList[worldX][worldY][worldZ];
	vector<Block*> blockList;
	vector<Block*> alphaBlockList;
	vector<Particle> particleList;
	void ObtainBlockList();
	BlockFactory blockInventory;

	void RenderBlocks();
	void RenderBlocks_GPass();
	void RenderEntities();
	void RenderEntities_GPass();
	void RenderScene();
	void RenderSkybox();
	void Render2D();

	unsigned nextUpdate;
	float RenderDist;
	Player player;

	void writeLine();
	float chatInputLimiter;
	bool shouldRenderChat;
	string line;

	bool Save(const char* filepath);
	bool Load(const char* filepath);
	bool GenerateWorld(Vector3 size);
};

#endif