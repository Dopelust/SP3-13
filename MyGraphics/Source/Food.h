#pragma once
#include <iostream>
#include <string>
#include "Item.h"

using namespace std;

class CFood : public CItem
{
private:
	unsigned HP;
public:
	CFood(void);
	~CFood(void);

	void setHP(unsigned);
	unsigned getHP(void);
};

