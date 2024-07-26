// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class USphereComponent;

UENUM(BlueprintType) //GE应用策略
enum class EEffectApplicationPolicy :uint8
{
	ApplyOnOverlap,
	ApplyOnEndoverlap,
	DoNotApply
};

UENUM(BlueprintType)//GE移除策略
enum class EEffectRemovalPolicy :uint8
{
	RemoveOnOverlap,
	RemoveOnEndoverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FEffectWithPolicy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy EffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy  InfiniteRemovalPolicy{ EEffectRemovalPolicy::DoNotRemove };
};

UCLASS()
class AURA_LEARN_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAuraEffectActor();
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void ApplyEffecToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GEClass);
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

protected:
	UPROPERTY(EditAnywhere)
	TArray<FEffectWithPolicy> EffectWithPolicys;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectRemoval{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	float ActorLevel{ 1.f };//GE应用的等级
private:

};
