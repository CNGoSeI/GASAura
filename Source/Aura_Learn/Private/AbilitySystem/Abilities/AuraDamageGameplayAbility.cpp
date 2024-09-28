#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemBPLibary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
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
