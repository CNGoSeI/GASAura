#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemBPLibary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* BeDamageActor)
{
	auto DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.F);
	for(const auto& Pair:DamageTypes)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(BeDamageActor));
}

FGameplayTag UAuraDamageGameplayAbility::GetCurrentAbilityTag() const
{
	for(const auto& GATag: AbilityTags)
	{
		if (GATag.MatchesTag(FAuraGmaeplayTags::GetInstance().Abilities))return GATag;
	}
	return FAuraGmaeplayTags::GetInstance().Abilities_None;
}

const FTaggedMontage& UAuraDamageGameplayAbility::GetRandomTaggedMontage(const TArray<FTaggedMontage>& Arrary) const
{
	check(Arrary.Num())
	return Arrary[FMath::RandRange(0, Arrary.Num() - 1)];
}

float UAuraDamageGameplayAbility::GetManaCost(const float Level) const
{
	float ManaCost{ 0.f };
	const auto CostGE = GetCostGameplayEffect(); //获取GA中花费GE类
	if (!CostGE)return ManaCost;


	for (const auto& ModifierInfo : CostGE->Modifiers)
	{
		////对Mana修改的修饰器 那一定是法力花销值
		if (ModifierInfo.Attribute == UAuraAttributeSet::GetManaAttribute())
		{
			ModifierInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
			break;
		}
	}

	return ManaCost;
}


float UAuraDamageGameplayAbility::GetCooldown(const float Level) const
{
	float CD{ 0.F };
	const auto CDGE = GetCooldownGameplayEffect();
	if (!CDGE)return CD;

	//CD 是持续时间的属性修改器中
	CDGE->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, CD);

	return CD;
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(const FGameplayTag& DamageType, const float Level) const
{
	if (!DamageTypes.Contains(DamageType))return 0.f;
	return DamageTypes[DamageType].GetValueAtLevel(Level);
}
