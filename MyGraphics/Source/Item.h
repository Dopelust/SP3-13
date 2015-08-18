#pragma once
#include <iostream>
#include <string>
#include "Mesh.h"

using namespace::std;

class CItem
{
protected:
	string name;
	Mesh* mesh;
	unsigned count;
	bool unique;
	
public:
	CItem(void);
	~CItem(void);

	virtual void Attack();
};

