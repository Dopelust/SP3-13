#include "Inventory.h"
#include "Application.h"

Slot::Slot() : item(NULL), next(NULL)
{
}
Slot::~Slot()
{
}

CInventory::CInventory(void) 
{
	for (unsigned i = 0; i < InventorySize; ++i)
		slot[i] = new Slot();

	for (unsigned i = 0; i < InventorySize; ++i)
	{
		if (i == InventorySize - 1)
		{
			slot[i]->next = slot[0];
			slot[i]->prev = slot[i - 1];
		}
		else
		{
			slot[i]->next = slot[i + 1];
			if (i == 0)
				slot[i]->prev = slot[InventorySize - 1];
			else
				slot[i]->prev = slot[i - 1];
		}
	}

	selectedSlot = slot[0];
}

CInventory::~CInventory(void)
{
}

void CInventory::Update(double dt)
{
	if (Application::mouseScroll != 0)
	{
		Scroll(Application::mouseScroll);
		Application::mouseScroll = 0;
	}

	for (unsigned i = 0; i < InventorySize; ++i)
	{
		if (Application::IsKeyPressed('1' + i))
			Select(i + 1);
	}

	if (selectedSlot->item)
		selectedSlot->item->Update(dt);
}

bool CInventory::Select(int select)
{
	if (select > 0 && select <= InventorySize)
	{
		selectedSlot = slot[select - 1];
		return true;
	}
	return false;
}
void CInventory::Scroll(int scroll)
{
	if (scroll < 0)
		selectedSlot = selectedSlot->next;
	else if (scroll > 0)
		selectedSlot = selectedSlot->prev;
}