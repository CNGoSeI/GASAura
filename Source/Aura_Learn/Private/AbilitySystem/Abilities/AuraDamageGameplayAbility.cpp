#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* BeDamageActor)
{
	auto DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.F);
	for(const auto& Pair:DamageTypes)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(BeDamageActor));
}

const FTaggedMontage& UAuraDamageGameplayAbility::GetRandomTaggedMontage(const TArray<FTaggedMontage>& Arrary) const
{
	check(Arrary.Num())
	return Arrary[FMath::RandRange(0, Arrary.Num() - 1)];
}
