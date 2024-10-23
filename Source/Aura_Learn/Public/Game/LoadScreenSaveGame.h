#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM()
enum ESaveSlotStatus:uint8 //�浵��۽��洦�����ֽ���ģʽ
{
	Vacant=0, // ��ȱ���ģʽ
	EnterName, //����浵��
	Taken, //��ֱ�Ӽ���
};

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

	UPROPERTY()
	FString MapName{ "Default Name" };

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus{Vacant};
};
