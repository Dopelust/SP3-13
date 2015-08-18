#pragma once
#include <vector>

#include "Item.h"

class Slot
{
public:
	Slot();
	~Slot();

	Slot* prev;
	Slot* next;
	CItem* item;
};

class CInventory
{
public:
	static const int InventorySize = 3;

	CInventory(void);
	~CInventory(void);

	void Update();
	bool Select(int select);
	void Scroll(int scroll);

	Slot* slot[InventorySize];
	Slot* selectedSlot;
};

