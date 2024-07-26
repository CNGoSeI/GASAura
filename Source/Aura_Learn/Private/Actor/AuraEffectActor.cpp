// 学习使用

#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffecToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GEClass)
{
	check(GEClass)
	auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))return;

	auto EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	auto EffectSpec = TargetASC->MakeOutgoingSpec(GEClass, ActorLevel, EffectContext);
	TargetASC->ApplyGameplayEffectSpecToSelf(*(EffectSpec.Data.Get()));
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	for (auto& it : EffectWithPolicys)
	{
		if (it.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffecToTarget(TargetActor, it.GameplayEffectClass);
		}
	}

}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	auto TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetAsc))return;

	for (auto& it : EffectWithPolicys)
	{
		if (it.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndoverlap)
		{
			ApplyEffecToTarget(TargetActor, it.GameplayEffectClass);
		}

		//移除无限GE
		if (it.InfiniteRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndoverlap)
		{
			TargetAsc->RemoveActiveGameplayEffectBySourceEffect(it.GameplayEffectClass, TargetAsc, 1); //移除一层
		}
	}
}
