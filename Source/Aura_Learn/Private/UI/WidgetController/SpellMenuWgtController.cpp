#include "UI/WidgetController/SpellMenuWgtController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWgtController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	if (auto AuraPS = GetAuraPS())
	{
		SpellPointChangeDel.Broadcast(AuraPS->GetSpellPoints());
	}
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

	if (auto AuraPS = GetAuraPS())
	{
		AuraPS->OnSpellPointsChangedDelegate.AddLambda([this](const int32 SpellPoint)
		{
			SpellPointChangeDel.Broadcast(SpellPoint);
		});
	}

}

void USpellMenuWgtController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const int32 SpellPoint = GetAuraPS()->GetSpellPoints();

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(FAuraGmaeplayTags::GetInstance().Abilities_None);//是否是占位技能标签
	auto GASpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = GASpec != nullptr;//找到可激活Tag

	FGameplayTag AbilityStatus;
	if(!bTagValid||bTagNone||!bSpecValid)//会出现未解锁的情况
	{
		AbilityStatus = FAuraGmaeplayTags::GetInstance().Abilities_Status_Locked;
	}else //其他情况技能已解锁 从GAS读取技能现有规格
	{
		AbilityStatus = GetAuraASC()->GetStatusFromSpec(*GASpec);
	}

	//判断按钮启用状态
	bool bShouldEnableSpellPointBtn{ false };
	bool bShouldEnableEquipBtn{ false };
	ShouldEnableBtns(AbilityStatus, SpellPoint, bShouldEnableSpellPointBtn, bShouldEnableEquipBtn);

	SpellGlobeSelectedDel.Broadcast(bShouldEnableSpellPointBtn, bShouldEnableEquipBtn);
}

void USpellMenuWgtController::ShouldEnableBtns(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                               bool& bShouldEnableSpellPointBtn, bool& bShouldEnbalEquipBtn)
{
	//技能解锁了都可以花点升级 但是装备按钮仅能在解锁和装备状态下启用
	if (AbilityStatus.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped))
	{
		bShouldEnbalEquipBtn = true;
		bShouldEnableSpellPointBtn = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Eligible))
	{
		bShouldEnbalEquipBtn = false;
		bShouldEnableSpellPointBtn = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Unlocked))
	{
		bShouldEnbalEquipBtn = true;
		bShouldEnableSpellPointBtn = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Locked))
	{
		bShouldEnbalEquipBtn = false;
		bShouldEnableSpellPointBtn = false;
	}
}
