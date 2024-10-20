#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AURA_LEARN_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString SlotName{};//��Ӧ�����еĲ��UI

	UPROPERTY()
	uint8 SlotIndex;

	UPROPERTY()
	FString PlayerName{"Default Name"};
};
