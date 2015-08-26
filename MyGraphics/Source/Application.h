
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
#include "vertex.h"
#include "MinScene.h"
#include "SceneShadow.h"

class Application
{
public:
	Application();
	~Application();

	void Init();
	void Run();
	void Exit();

	static void setWindowSize(double w, double h);
	static int m_width;
	static int m_height;

	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short button);
	bool getKeyboardUpdate();
	bool getMouseUpdate();

	static void GetCursorPos(double *xpos, double *ypos);

	static int mouseScroll;

	static float textspace[255];
	static float getTextWidth(string t);

	static void ResetCursorPos();
	static void HideCursor(bool hideCursor);

	static StopWatch m_timer;
private:
	//Declare a window object
	double m_dElapsedTime;
	double m_dAccumulatedTime_ThreadOne;
	double m_dAccumulatedTime_ThreadTwo;

	SceneBase* scene;
};

#endif