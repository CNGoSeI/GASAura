// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;

/**飞射物品类基 */
UCLASS()
class AURA_LEARN_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();
	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere);
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	UPROPERTY(Replicated)
	TObjectPtr<USceneComponent>HomingTargetSceneCmpt;

	UPROPERTY(Replicated)
	FVector HomingTargetLocation;

	UPROPERTY(Replicated)
	bool bHomingProjectile = false;//是否启用制导

	UPROPERTY(Replicated)
	float HomingAccelerationMagnitude = 800.f;//加速度

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlapPrimitiveComponent, AActor* OtherActor,
	                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                     const FHitResult& SweepResult);
private:

	void OnHit();

	bool bHit{ false };//是否已经处理碰撞到目标之后的事件

	UPROPERTY(EditAnywhere)
	float LifeSpan{ 15.f };//该ACtor最大生命周期

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;//飞射物击中目标的特效

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ProjectileSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ProjectileLoopSoundCmpt{ nullptr };

};
