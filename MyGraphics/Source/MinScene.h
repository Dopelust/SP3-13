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

struct Button
{
	Button() : selected(false) {}

	Vector3 position;
	Vector3 scale;
	float textScale;
	float min_textScale;
	float max_textScale;
	Vector3 hitbox;
	string name;

	bool canSelect;
	bool selected;
};

class MinScene : public SceneShadow
{
public:
	enum MinState
	{
		NULL_STATE,
		MENU,
		PLAY,
		CREDITS,
	};

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
	Particle* FetchParticle(unsigned id);
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
	vector<Particle*> particleList;
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
	int ZoneCounter;
	bool InTheZone;

	bool showMap;
	Waypoint playerWaypoint;
	vector<Waypoint> waypointList;
	void UnleashTheHounds();

	ISound* menuSound;
	Camera3 menuCamera;
	vector<Button> menuTitle;
	vector<Button> menuButtons;
	MinState GameState;
	MinState QueuedState;
	
	vector<Mtx44> WaterMMat;
	float screenFade;

	//Popup anim
	bool showPopUp;
	bool PopUpFullyLoaded;
	bool bfLoaded;
	float PopUpAnimTimer;

	//Loading bools
	bool loadCastleGateText;
	bool loadGreeting;
	bool loadTutorialText1;
	bool loadOutpostText;
	bool loadOutpostExit;
	bool loadLionGreeting;
	bool loadLionExit;
	bool loadWinterfellGreeting;

	//Etc variables
	double TutorialTimer;

	//Text loading functions
	void LocationTriggers(double dt);
	void PopUpAnim(double dt);
	void DirectionCheck(double dt);
	void InitTextStuff();
	bool LoadTextSpeech(const char* filepath);
	void RenderPopUpBox();
};

#endif
