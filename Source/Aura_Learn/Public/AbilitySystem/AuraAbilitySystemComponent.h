// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer&/*AssetTags*/)

UCLASS()
class AURA_LEARN_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSeted();//AvatarActor和OwnerActor 设置完成之后的调用

	FEffectAssetTags EffectAssetTagsDel;
protected:
	//被GE应用自身时的广播绑定
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle);

};
