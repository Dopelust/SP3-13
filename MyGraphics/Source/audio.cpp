/******************************************************************************/
/*!
\file	audio.cpp
\author SP Team 16
\par	
\brief
this is for the audio
*/
/******************************************************************************/
#include "MinScene.h"

ISoundEngine * engine;

/******************************************************************************/
/*!
		Class MinScene::soundInit
\brief	the sound initializer for the Project scene
*/
/******************************************************************************/
int MinScene::soundInit()
{
	engine = createIrrKlangDevice();
	engine->setSoundVolume(1.f);
	engine->setDefault3DSoundMinDistance(1);

   if (!engine)
      return 0;

   return 0;
}

/******************************************************************************/
/*!
		Class MinScene::soundUpdate
\brief	This is the sound update for playing diff sounds
*/
/******************************************************************************/
int MinScene::soundUpdate(Player& player)
{
	Vector3 direction;
	direction.SphericalToCartesian(player.hOrientation - 180, player.vOrientation);
	engine->setListenerPosition(vec3df(player.position.x,player.position.y,player.position.z), vec3df(direction.x, direction.y, direction.z));
	
	return 0;
}

int MinScene::soundExit()
{
	if(engine)
		engine->drop(); 
	return 0;
}