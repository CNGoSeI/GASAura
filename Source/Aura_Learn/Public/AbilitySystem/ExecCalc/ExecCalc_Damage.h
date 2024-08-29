﻿// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "ExecCalc_Damage.generated.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false)
	};
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UCLASS()
class AURA_LEARN_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                            FGameplayEffectCustomExecutionOutput& OutExecutionOutput)const override;
};
