// 学习使用


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/AuraGameplayAbility.h"

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
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}

}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	//遍历 拿到可以激活的GA
	for(auto& GASpec:GetActivatableAbilities())
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
