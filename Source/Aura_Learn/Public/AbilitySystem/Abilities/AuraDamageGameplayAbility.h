﻿#pragma once
#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 *  比项目基类GA仅多了伤害相关
 */

typedef TMap<FGameplayTag, FDamageGEParamsByDamageType> TDamageType2Params;//伤害类型对应参数的类型定义

UCLASS()
class AURA_LEARN_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* BeDamageActor);
	FGameplayTag GetCurrentAbilityTag()const;
	const TMap<FGameplayTag, FScalableFloat>& GetDamageTypes()const;

	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor=nullptr);//仅填充部分拥有，需要自定化
protected:

	void MakeDeBuffGEParams(TMap<FGameplayTag, FDamageGEParamsByDamageType>& DebuffMapParams);//DebuffGE的各项需要的参数

	/*
	 * 保留了一个技能存在多种属性伤害的功能 但是其他属性的debuff 伤害和机会另写一个类，从该类 生成一个GE 数组 进行返回
	 */

	UFUNCTION(BlueprintCallable,BlueprintPure)
	const FTaggedMontage& GetRandomTaggedMontage(const TArray<FTaggedMontage>& Arrary)const;//从给定的标签蒙太奇组随机返回一个

protected:
	float GetManaCost(const float Level = 1.f) const;//获取技能的魔力消耗
	float GetCooldown(const float Level = 1.f) const;//获取技能冷却
	float GetDamageByDamageType(const FGameplayTag& DamageType, const float Level = 1.f) const;

	UPROPERTY(EditDefaultsOnly, DisplayName="死亡冲击力度")
	float DeathImpulseMagnitude{60.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat>DamageTypes;//对应属性伤害的值


};
