#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangeDel, int32);

UCLASS()
class AURA_LEARN_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override;
	UAttributeSet* GetAttributeSet()const { return AttributeSet; };

	FORCEINLINE int16 GetPlayerLevel() const ;
	FORCEINLINE int32 GetPlayerXP() const;

	UFUNCTION(BlueprintCallable)
	void SetXP(const int32 InXP);

	UFUNCTION(BlueprintCallable)
	void AddToXP(const int32 InXP);

	UFUNCTION(BlueprintCallable)
	void SetLevel(const int32 InLevel);

	UFUNCTION(BlueprintCallable)
	void AddToLevel(const int32 InLevel);

	FOnPlayerStateChangeDel OnXPChangeDel;
	FOnPlayerStateChangeDel OnLevelChangeDel;

	UPROPERTY(EditDefaultsOnly,DisplayName="升级信息资产表")
	TObjectPtr<ULevelUpInfo>DA_LevelInfo;
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_Level)
	int16 Level{ 1 };

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP{ 1 };

	UFUNCTION()
	void OnRep_Level(int16 OldValue);

	UFUNCTION()
	void OnRep_XP(int32 OldValue);

};
