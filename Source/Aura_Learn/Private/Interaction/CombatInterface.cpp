// 学习使用


#include "Interaction/CombatInterface.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.
int32 ICombatInterface::GetPlayerLevel()
{
	return 0;
}

FHitResult* ICombatInterface::GetCursorHitRes()
{
	return nullptr;
}
