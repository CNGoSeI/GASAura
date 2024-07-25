// 学习使用


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

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
	auto TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(!IsValid(TargetASC))return;
	auto EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	auto EffectSpec=TargetASC->MakeOutgoingSpec(GEClass, 1.f, EffectContext);
	TargetASC->ApplyGameplayEffectSpecToSelf(*(EffectSpec.Data.Get()));
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if(InstantEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffecToTarget(TargetActor, InstantGameplayEffectClass);
	}
	//TODO:: 还有无限和持续的  但是也许能重构减少重复，暂时没看见多个effective类的必要性
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if(InstantEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnEndoverlap)
	{
		ApplyEffecToTarget(TargetActor, InstantGameplayEffectClass);
	}
}
