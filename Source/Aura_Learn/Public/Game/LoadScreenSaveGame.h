#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM()
enum ESaveSlotStatus:uint8 //存档插槽界面处于哪种界面模式
{
	Vacant=0, // 空缺插槽模式
	EnterName, //输入存档名
	Taken, //能直接加载
};

/**
 * 存档，无多说
 */
UCLASS()
class AURA_LEARN_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString SlotName{};//对应界面中的插槽UI

	UPROPERTY()
	uint8 SlotIndex;

	UPROPERTY()
	FString PlayerName{"Default Name"};

	UPROPERTY()
	FString MapName{ "Default Name" };

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus{Vacant};

	UPROPERTY()
	bool bFirstTimeLoadIn{ true };//是否为第一次载入

	/* 玩家信息 */
	UPROPERTY()
	uint16 PlayerLevel{ 1 };

	UPROPERTY()
	uint32 PlayerXP{ 0 };

	UPROPERTY()
	uint16 SpellPoint{ 0 };//技能点

	UPROPERTY()
	uint16 AttributePoint{ 0 };

	UPROPERTY()
	float Strength{ 0 };

	UPROPERTY()
	float Vigor{ 0 };

	UPROPERTY()
	float Resilience{ 0 };

	UPROPERTY()
	float Intelligence{ 0 };

	/* 玩家信息 */
};
