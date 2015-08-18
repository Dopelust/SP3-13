#include "Food.h"


CFood::CFood(void) : HP(0)
{
}

CFood::~CFood(void)
{
}

void CFood::setHP(unsigned HP)
{
	this->HP = HP;
}

unsigned int CFood::getHP(void)
{
	return HP;
}
