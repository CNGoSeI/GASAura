// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
/*单例*/

struct FAuraGmaeplayTags
{

public:
	static FAuraGmaeplayTags& GetInstance();
	static void InitNaviveGameplayTags();

	FGameplayTag Attribute_Secondary_Armor;
protected:

private:

};
