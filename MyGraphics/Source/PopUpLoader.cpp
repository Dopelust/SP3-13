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


void MinScene::InitTextStuff()
{
	showPopUp = false;
	PopUpFullyLoaded = false;
	PopUpAnimTimer = 0.f;
	bfLoaded = false;
	TutorialTimer = 0.f;

	loadCastleGateText = false;
	loadGreeting = false;
	loadTutorialText1 = false;
	loadOutpostText = false;
	loadOutpostExit = false;
	loadLionGreeting = false;
	loadLionExit = false;
	loadWinterfellGreeting = false;
}

void MinScene::LocationTriggers(double dt)
{
	PopUpAnim(dt);
	TutorialTimer += dt * 1.5;
	
	if (TutorialTimer > 0 && TutorialTimer < 5)							//Greeting
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadGreeting = true;
			bfLoaded = true;
		}
	}
	
	else if (TutorialTimer > 7 && TutorialTimer < 15)					//Tut1
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadTutorialText1 = true;
			bfLoaded = true;
		}
	}

	else if (player.position.x > 57 && player.position.x < 61 &&
			 player.position.z > -2 && player.position.z < 3)					//Castle Black(going out)
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadCastleGateText = true;
			bfLoaded = true;
		}
	}
	else if (player.position.x > -30 && player.position.x < 30 &&
			 player.position.z > -59 && player.position.z < -51)				//Outpost greeting
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadOutpostText = true;
			bfLoaded = true;
		}
	}

	else if (player.position.x > -26 && player.position.x < 25.6	 &&
			 player.position.z > -62.9 && player.position.z < -59)				//Outpost gate
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadOutpostExit = true;
			bfLoaded = true;
		}
	}

	else if (player.position.x > -40 && player.position.x < 0 &&
			 player.position.z > 32 && player.position.z < 39.5)				//Lion greeting
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadLionGreeting = true;
			bfLoaded = true;
		}
	}

	else if (player.position.x > -24 && player.position.x < -12 &&
			 player.position.z > 39.3 && player.position.z < 43.5)				//Lion exit
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadLionExit = true;
			bfLoaded = true;
		}
	}

	else if (player.position.x > -78 && player.position.x < -70 &&
			 player.position.z > -40 && player.position.z < 40)				//Winterfell greeting
	{
		showPopUp = true;
		if (PopUpFullyLoaded == true && bfLoaded == false)
		{
			loadWinterfellGreeting = true;
			bfLoaded = true;
		}
	}

	else
	{
		PopUpAnimTimer = 0;
		showPopUp = false;
		PopUpFullyLoaded = false;
		bfLoaded = false;


		loadCastleGateText = false;
		loadGreeting = false;
		loadTutorialText1 = false;
		loadOutpostText = false;
		loadOutpostExit = false;
		loadLionGreeting = false;
		loadLionExit = false;
		loadWinterfellGreeting = false;
	}
}

void MinScene::PopUpAnim(double dt)
{
	if (showPopUp == true && PopUpFullyLoaded == false)
	{
		PopUpAnimTimer += dt * 2.f;
		if (PopUpAnimTimer >= 0.55f)
		{
			PopUpAnimTimer += 0;
			PopUpFullyLoaded = true;
		}
	}
}

bool MinScene::LoadTextSpeech(const char * filepath)
{
	string line;
	//vector
	vector<string> data;
	ifstream myfile;
	myfile.open(filepath);

	
	if (!myfile.eof())
	{
		while(getline(myfile, line))
		{
			data.push_back(line);
		}
		myfile.close();

		//cout << "Succesfully opened: " << filepath << endl;
		for(int n = 0; n < data.size(); n++)
		{
			ostringstream ss;
			ss << data[n];

			modelStack.PushMatrix();
			modelStack.Translate(-2,-2, 0);
			modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.3f - n * 35, 0);
			modelStack.Scale(24);
			modelStack.Translate(-Application::getTextWidth(ss.str()) * 0.5f, 0, 0);
			RenderText(ss.str(), Color(0,0,0));
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(Application::m_width * 0.5f, Application::m_height * 0.3f - n * 35, 0);
			modelStack.Scale(24);
			modelStack.Translate(-Application::getTextWidth(ss.str()) * 0.5f, 0, 0);
			RenderText(ss.str(), Color(1, 1, 1));
			modelStack.PopMatrix();

		}
		return true;
	}

	cout << "Failed to open: " << filepath << endl;
	return false;
}

void MinScene::RenderPopUpBox()
{
	//modelStack.PushMatrix();
	//modelStack.Translate(Application::m_width - 100, Application::m_height - 300, 0);
	//modelStack.Translate(0, Application::m_height * (-0.35f), 0);
	//modelStack.Scale(512 * PopUpAnimTimer, 256, 1);
	//meshList["QUAD"]->textureID = textureID["POPUP"];  
	//RenderMesh(meshList["QUAD"], false, Color(0.68f,0.68f,0.68f));
	//modelStack.PopMatrix();

	if (loadCastleGateText == true)
		LoadTextSpeech("Assets//Text//CastleBlackGate.txt");
	else if (loadGreeting == true)
		LoadTextSpeech("Assets//Text//Greeting.txt");
	else if (loadTutorialText1 == true)
		LoadTextSpeech("Assets//Text//Tutorial1.txt");
	else if (loadOutpostText == true)
		LoadTextSpeech("Assets//Text//OutpostGreeting.txt");
	else if (loadOutpostExit == true)
		LoadTextSpeech("Assets//Text//OutpostExit.txt");
	else if (loadLionGreeting == true)
		LoadTextSpeech("Assets//Text//LionGreeting.txt");
	else if (loadLionExit == true)
		LoadTextSpeech("Assets//Text//LionExit.txt");
	else if (loadWinterfellGreeting == true)
		LoadTextSpeech("Assets//Text//WinterfellGreeting.txt");
}