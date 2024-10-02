#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemBPLibary.h"
#include "AbilitySystem/AuraGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura_Learn/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSeted()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClinetEffectApplied);

	auto GetChance = [](const int16 PlayerLevel, const int16 GALevel, const float Damage)-> float
		{

			if (Damage < 10)return false; //如果不想触发，则让伤害小于10
			float Coefficient = (Damage - 9) * 2 + PlayerLevel + GALevel;
			//系数算下来在 0-1之间 *100 进行扩张
			return (Coefficient / (Coefficient + 100)) * 100;
		};

	DamageTypeMapSetParamFunction.Emplace(FAuraGmaeplayTags::GetInstance().Damage_Fire, [GetChance](const int16 PlayerLevel, const int16 AbilityLevel, const float BaseDamage)->FDamageGEParamsByDamageType
	{
			FDamageGEParamsByDamageType Info{};

			Info.DamageType = FAuraGmaeplayTags::GetInstance().Damage_Fire;
			Info.BaseDamage = BaseDamage;
			Info.DebuffDamage = (BaseDamage/10.f)+ AbilityLevel;
			Info.DebuffDuration = (AbilityLevel + PlayerLevel) / 8 + Info.DebuffDamage+3;
			Info.DebuffChance = GetChance(PlayerLevel, AbilityLevel, BaseDamage)+98;
			return Info;
	});

	DamageTypeMapSetParamFunction.Emplace(FAuraGmaeplayTags::GetInstance().Damage_Arcane, [GetChance](const int16 PlayerLevel, const int16 AbilityLevel, const float BaseDamage)->FDamageGEParamsByDamageType
		{
			FDamageGEParamsByDamageType Info{};

			Info.DamageType = FAuraGmaeplayTags::GetInstance().Damage_Arcane;
			Info.BaseDamage = BaseDamage;
			Info.DebuffDamage = (BaseDamage / 6.f) + AbilityLevel;
			Info.DebuffDuration = (AbilityLevel + PlayerLevel) / 10 + Info.DebuffDamage + 2;
			Info.DebuffChance = GetChance(PlayerLevel, AbilityLevel, BaseDamage);
			return Info;
		});

	DamageTypeMapSetParamFunction.Emplace(FAuraGmaeplayTags::GetInstance().Damage_Physical, [GetChance](const int16 PlayerLevel, const int16 AbilityLevel, const float BaseDamage)->FDamageGEParamsByDamageType
		{
			FDamageGEParamsByDamageType Info{};

			Info.DamageType = FAuraGmaeplayTags::GetInstance().Damage_Physical;
			Info.BaseDamage = BaseDamage;
			Info.DebuffDamage = (BaseDamage / 8.f) + AbilityLevel;
			Info.DebuffDuration = (AbilityLevel + PlayerLevel) / 7 + Info.DebuffDamage + 3;
			Info.DebuffChance = GetChance(PlayerLevel, AbilityLevel, BaseDamage);
			return Info;
		});

	DamageTypeMapSetParamFunction.Emplace(FAuraGmaeplayTags::GetInstance().Damage_Lightning, [GetChance](const int16 PlayerLevel, const int16 AbilityLevel, const float BaseDamage)->FDamageGEParamsByDamageType
		{
			FDamageGEParamsByDamageType Info{};

			Info.DamageType = FAuraGmaeplayTags::GetInstance().Damage_Lightning;
			Info.BaseDamage = BaseDamage;
			Info.DebuffDamage = (BaseDamage / 2.f) + AbilityLevel;
			Info.DebuffDuration = FMath::Min((AbilityLevel + PlayerLevel / 5) / 10 + Info.DebuffDamage / 10 + 1,6.f);
			Info.DebuffChance = GetChance(PlayerLevel, AbilityLevel, BaseDamage);
			return Info;
		});
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{

	for(auto& AbilityClass: Abilities)
	{
		auto AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.f);
		if (const auto AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);//将输入Tag添加进DynamicAbilityTags
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped);//已经装备
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGiveDel.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities)
{
	for (auto& AbilityClass : PassiveAbilities)
	{
		auto AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	//遍历 拿到可以激活的GA
	for(auto& GASpec:GetActivatableAbilities())//GetActivatableAbilities返回的变量有客户端收到回调的函数版本，因此客户端的需要的相应内容需要在onrep函数处理
	{
		if(GASpec.DynamicAbilityTags.HasTag(InputTag))
		{

			AbilitySpecInputPressed(GASpec);//告知GA 目前操作状态在 Pressed下

			if(!GASpec.IsActive())//非激活状态才进行激活 防止输入每帧调用
			{
				TryActivateAbility(GASpec.Handle);//激活
			}
		}
	}

}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	//遍历 拿到可以激活的GA
	for (auto& GASpec : GetActivatableAbilities())
	{
		if (GASpec.DynamicAbilityTags.HasTag(InputTag))
		{

			AbilitySpecInputReleased(GASpec);//告知GA 目前操作状态在 已经取消

		}
	}
}

void UAuraAbilitySystemComponent::ClinetEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	GameplayEffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTagsDel.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopedLock(*this);//临时锁定能力列表，防止能力列表在执行特定操作时发生变化,是自动锁机制
	for(const auto& AbilitySpec:GetActivatableAbilities())
	{
		//执行代理的函数，在UOverlayWidgetController::OnInitStartupAbilities 中 代理的函数设置为根据能力Tag查找DA获取能力相关的UI信息，并且进行广播
		if(!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Warning, TEXT("Failed to Excute delegate in %hs ") , __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& GASpec)
{
	if (!IsValid(GASpec.Ability))return FGameplayTag{};
	for(const auto& Tag: GASpec.Ability->AbilityTags)
	{
		if(Tag.MatchesTag(FAuraGmaeplayTags::GetInstance().Abilities)	)
		{
			return Tag;
		}
	}
	return FGameplayTag{};
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& GASpec)
{
	if (!IsValid(GASpec.Ability))return FGameplayTag{};
	for (const auto& Tag : GASpec.DynamicAbilityTags)//与该能力实例相关的 动态 Gameplay Tags
	{
		if (Tag.MatchesTag(FAuraGmaeplayTags::GetInstance().InputTag))
		{
			return Tag;
		}
	}
	return FGameplayTag{};
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& GASpec)
{
	if (!IsValid(GASpec.Ability))return FGameplayTag{};
	for (const auto& Tag : GASpec.DynamicAbilityTags)//与该能力实例相关的 动态 Gameplay Tags
	{
		if (Tag.MatchesTag(FAuraGmaeplayTags::GetInstance().Abilities_Status))
		{
			return Tag;
		}
	}
	return FGameplayTag{};
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (auto Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag{};
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if(auto Spec=GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}
	return FGameplayTag{};
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopedLock(*this);//上锁

	//从可激活的所有GA内的Tag中找到对应技能Tag
	for(auto& GASpec:GetActivatableAbilities())
	{
		for(const auto& GATag:GASpec.Ability->AbilityTags)
		{
			if(AbilityTag.MatchesTagExact(GATag))
			{
				return &GASpec;
			}

		}
	}

	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if(IPlayerInterface::Execute_GetCurrentAttributePoint(GetAvatarActor())>0)
		{
			ServerUpgradeAttribute(AttributeTag);//主要是为了防止客户端没有点数调用服务器执行而浪费资源，因此先让客户端判断，再通知服务器升级
		}
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatus(const int32 Level)
{
	auto AbilityInfo=UAuraAbilitySystemBPLibary::GetAbilityInfo(GetAvatarActor());

	for(const auto& Info:AbilityInfo->AbilityInfomation)
	{
		if(Level < Info.LevelRequirement||!Info.AbilityTag.IsValid())continue;

		//等级够 并且 非可激活状态(GAS 中没有该技能) 才进行更改
		if(GetSpecFromAbilityTag(Info.AbilityTag)==nullptr)
		{
			auto FutureAddGASpec = FGameplayAbilitySpec(Info.Ability,1);
			FutureAddGASpec.DynamicAbilityTags.AddTag(FAuraGmaeplayTags::GetInstance().Abilities_Status_Eligible);
			GiveAbility(FutureAddGASpec);

			MarkAbilitySpecDirty(FutureAddGASpec);//标记能力规格已被修改 强制现在更新

			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGmaeplayTags::GetInstance().Abilities_Status_Eligible);//客户端广播技能状态改变
		}
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag)
{
	if (auto GASpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const auto PrevSlot = GetInputTagFromSpec(*GASpec); //该技能之前的 输入槽位
		const auto Status = GetStatusFromSpec(*GASpec);

		//技能状态是能操作的 已解锁||已经整备
		const bool bIsValidStatus =
			Status.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Unlocked) ||
			Status.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped);

		if (bIsValidStatus)
		{
			//清除该槽上的技能 （把该槽上技能的 输入Tag 移除）
			ClearAbilitiesOfSlot(SlotTag);
			//防止该技能已经有其他槽标签 因此也进行移除其输入Tag
			ClearSlot(GASpec);

			//将槽位标签赋予给该技能
			GASpec->DynamicAbilityTags.AddTag(SlotTag);

			//如果是已解锁状态 整备之后要改为已经整备状态
			if(Status.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Unlocked))
			{
				GASpec->DynamicAbilityTags.RemoveTag(Status);
				GASpec->DynamicAbilityTags.AddTag(FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped);

			}else if(Status.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped))//如果之前已经整备，那么清除之前的整备槽
			{
				
			}
			MarkAbilitySpecDirty(*GASpec);

			ClientEquipAbility(AbilityTag, SlotTag, PrevSlot);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
                                                                    const FGameplayTag& TargetSlotTag,
                                                                    const FGameplayTag& PrevSlotTag)
{
	AbilityEquippedDel.Broadcast(AbilityTag, FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped, TargetSlotTag, PrevSlotTag);

}

bool UAuraAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& Description, FString& NextLevelDescription)
{
	Description = FString("");
	NextLevelDescription = FString("");

	//if(!GetAvatarActor()->HasAuthority())return false;
	if(const auto& GASpec=GetSpecFromAbilityTag(AbilityTag))
	{
		if(auto AuraAbility=Cast<UAuraGameplayAbility>(GASpec->Ability))
		{
			Description = AuraAbility->GetDescription(this, AbilityTag,GASpec->Level);
			NextLevelDescription= AuraAbility->GetDescription(this, AbilityTag, GASpec->Level+1);
			return true;
		}

	}

	if(auto AbilityInfo = UAuraAbilitySystemBPLibary::GetAbilityInfo(GetAvatarActor()))
	{
		Description = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}

	return false;
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for(const auto& Tag:Spec->DynamicAbilityTags)
	{
		if(Slot.MatchesTagExact(Tag))
		{
			return true;
		}
	}

	return false;
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* GASpec)
{
	if(GASpec)
	{
		const auto Slot = GetInputTagFromSpec(*GASpec);
		GASpec->DynamicAbilityTags.RemoveTag(Slot);

		MarkAbilitySpecDirty(*GASpec);
	}
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& SlotTag)
{
	FScopedAbilityListLock Locked(*this);

	for(auto& Spec:GetActivatableAbilities())
	{
		if(const bool bHasSlot=AbilityHasSlot(&Spec, SlotTag))
		{
			ClearSlot(&Spec);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if(auto GASpec=GetSpecFromAbilityTag(AbilityTag))
	{
		auto GAStatus = GetStatusFromSpec(*GASpec);

		//升级或者解锁
		if(GAStatus.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Eligible))//可以学习
		{
			//技能只允许有一个状态标签 先删再加
			GASpec->DynamicAbilityTags.RemoveTag(FAuraGmaeplayTags::GetInstance().Abilities_Status_Eligible);
			GASpec->DynamicAbilityTags.AddTag(FAuraGmaeplayTags::GetInstance().Abilities_Status_Unlocked);

			GAStatus = FAuraGmaeplayTags::GetInstance().Abilities_Status_Unlocked;
		}
		else if (GAStatus.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Unlocked)
			|| GAStatus.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped)) //可升级
		{
			GASpec->Level += 1;
		}

		//扣点
		if(GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		ClientUpdateAbilityStatus(AbilityTag, GAStatus, GASpec->Level);
		MarkAbilitySpecDirty(*GASpec);
	}

}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	/**
	 * 向监听事件的GA发送Tag事件，以达到升级的目的 (GA_ListenForEvent)
	 * 该GA应用的GE添加各种Magnitude 并且设置标签 SetByCall 因此能通过标签调用对应的Magnitude 修改对应的值
	 */

	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude=1.F;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	//升级之后得减一点
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& GATag,
                                                                           const FGameplayTag& StatusTag,
                                                                           int32 GALevel)
{
	AbilityStatusChangeDel.Broadcast(GATag, StatusTag, GALevel);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGiveDel.Broadcast(); //客户端
	}
	
}
