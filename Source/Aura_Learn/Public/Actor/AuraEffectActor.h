// 学习使用

#pragma once

class USphereComponent;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

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
	ApplyOnEndoverlap,
	DoNotRemove
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectRemoval{ false };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy{EEffectApplicationPolicy::DoNotApply};

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy  InfiniteRemovalPolicy{EEffectRemovalPolicy::DoNotRemove};
private:

};
