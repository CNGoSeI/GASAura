#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraGameplayAbility.h"
#include "Aura_Learn/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSeted()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClinetEffectApplied);
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

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGiveDel.Broadcast(); //客户端
	}
	
}
