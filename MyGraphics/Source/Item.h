#pragma once
#include <iostream>
#include <string>
#include "Mesh.h"
#include "MatrixStack.h"

using namespace::std;

class CItem
{
public:
	CItem(void);
	~CItem(void);

	string name;
	Mesh* mesh;
	unsigned count;
	bool unique;
	bool use;

	virtual void Update(double dt);
	virtual void RenderItem(MS& modelStack);
};

