#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewMode/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, uint8 SlotIdx)
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
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIdx);
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
