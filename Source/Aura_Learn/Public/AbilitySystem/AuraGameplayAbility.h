#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class UAuraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_LEARN_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly,Category="Input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(const UAuraAbilitySystemComponent* AuraGAS,const FGameplayTag& GATag,const int32 Level);//获取技能的文本描述
	static FString GetLockedDescription(const int32 Level);

protected:
	float GetManaCost(const float Level = 1.f);//获取技能的魔力消耗
	float GetCooldown(const float Level = 1.f);//获取技能冷却
};
