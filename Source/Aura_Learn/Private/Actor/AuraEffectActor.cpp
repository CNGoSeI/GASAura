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
	auto TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(!IsValid(TargetASC))return;
	auto EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	auto EffectSpec=TargetASC->MakeOutgoingSpec(GEClass, 1.f, EffectContext);
	TargetASC->ApplyGameplayEffectSpecToSelf(*(EffectSpec.Data.Get()));
}

