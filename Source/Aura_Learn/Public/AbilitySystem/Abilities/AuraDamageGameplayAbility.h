// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"


/**
 *  比项目基类GA仅多了伤害相关
 */
UCLASS()
class AURA_LEARN_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* BeDamageActor);
	FGameplayTag GetCurrentAbilityTag()const;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	TMap<FGameplayTag, FScalableFloat>DamageTypes;//对应属性伤害的值

	UFUNCTION(BlueprintCallable,BlueprintPure)
	const FTaggedMontage& GetRandomTaggedMontage(const TArray<FTaggedMontage>& Arrary)const;//从给定的标签蒙太奇组随机返回一个
};
