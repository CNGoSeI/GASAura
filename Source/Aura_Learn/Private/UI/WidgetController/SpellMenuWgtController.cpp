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
	GetAuraASC()->AbilityStatusChangeDel.AddLambda([this] (const FGameplayTag& GATag, const FGameplayTag& StatusTag,int32 GALevel)
	{
		//技能界面的按钮状态由选择时更改，如果选择未改变 但是状态改变了，也得进行广播通知
		if (GATag.MatchesTagExact(SelectAbility.Ability))
		{
			SelectAbility.Status = StatusTag;
			
			//技能状态改变得通知界面按钮
			UpdateAbilityWgtStatus(GATag,StatusTag, CurrentSpellpoint);

		}

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
			CurrentSpellpoint = SpellPoint;

			//技能点改变也得通知界面按钮更新状态
			UpdateAbilityWgtStatus(SelectAbility.Ability, SelectAbility.Status, CurrentSpellpoint);
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

	//缓存此次选中时的状态
	SelectAbility.Ability = AbilityTag;
	SelectAbility.Status = AbilityStatus;

	//判断按钮启用状态
	UpdateAbilityWgtStatus(AbilityTag, AbilityStatus, SpellPoint );
}

void USpellMenuWgtController::SpendPointBtnPressed()
{
	if(!GetAuraASC())return;
	GetAuraASC()->ServerSpendSpellPoint(SelectAbility.Ability);
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

void USpellMenuWgtController::UpdateAbilityWgtStatus(const FGameplayTag& GATag, const FGameplayTag& StatusTag, const int32 SpellPoint)
{
	bool bShouldEnableSpellPointBtn{ false };
	bool bShouldEnableEquipBtn{ false };
	ShouldEnableBtns(StatusTag, CurrentSpellpoint, bShouldEnableSpellPointBtn, bShouldEnableEquipBtn);

	FString Description{ "" }, NextLevelDescription{ "" };
	GetAuraASC()->GetDescriptionByAbilityTag(GATag, Description, NextLevelDescription);

	SpellGlobeSelectedDel.Broadcast(bShouldEnableSpellPointBtn, bShouldEnableEquipBtn, Description, NextLevelDescription);
}
