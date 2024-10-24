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
 * �浵���޶�˵
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
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus{Vacant};

	UPROPERTY()
	bool bFirstTimeLoadIn{ true };//�Ƿ�Ϊ��һ������

	/* �����Ϣ */
	UPROPERTY()
	uint16 PlayerLevel{ 1 };

	UPROPERTY()
	uint32 PlayerXP{ 0 };

	UPROPERTY()
	uint16 SpellPoint{ 0 };//���ܵ�

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

	/* �����Ϣ */
};
