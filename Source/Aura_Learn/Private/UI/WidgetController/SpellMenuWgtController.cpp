#include "UI/WidgetController/SpellMenuWgtController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void USpellMenuWgtController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
}

void USpellMenuWgtController::BindCallbackToDependencies()
{
	GetAuraASC()->AbilityStatusChangeDel.AddLambda([this] (const FGameplayTag& GATag, const FGameplayTag& StatusTag)
	{
		if(AbilityInfo)
		{
			auto GAInfo = AbilityInfo->FindAbilityInfoForTag(GATag);
			GAInfo.StatusTag = StatusTag;
			AbilityInfoDel.Broadcast(GAInfo);
		}
	});
}
