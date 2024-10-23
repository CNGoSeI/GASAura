#include "Game/AuraGameModeBase.h"

#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewMode/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, uint8 SlotIdx) const
{
	if(UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(),SlotIdx))
	{
		//已经存在的存档 删除之后再保存一个新的
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIdx);
	}

	auto SaveGameObj=UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	auto LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObj);

	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotStatus = ESaveSlotStatus::Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIdx);
}

void AAuraGameModeBase::DeleteSlotData(UMVVM_LoadSlot* LoadSlot, uint8 SlotIdx)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIdx))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIdx);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIdx) const
{
	USaveGame* SaveGameObj{ nullptr };
	if(UGameplayStatics::DoesSaveGameExist(SlotName, SlotIdx))
	{
		SaveGameObj=UGameplayStatics::LoadGameFromSlot(SlotName, SlotIdx);

	}else
	{
		SaveGameObj = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	return Cast<ULoadScreenSaveGame>(SaveGameObj);

}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, NameToMap.FindChecked(Slot->GetMapName()));
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	NameToMap.Emplace(DefaultMapName, DefaultMap);
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	auto AuraGI = Cast<UAuraGameInstance>(GetGameInstance());

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				//根据Tag找到所需的起始点
				if (PlayerStart->PlayerStartTag == AuraGI->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}
