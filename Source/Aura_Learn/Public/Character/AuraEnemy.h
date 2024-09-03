// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class AAuraAIController;
class UBehaviorTree;
class UWidgetComponent;
/**
 *
 */
UCLASS()
class AURA_LEARN_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void HighlightActor() override;
	virtual void UnHightlightActor() override;

	FORCEINLINE virtual int32 GetPlayerLevel() override;

	virtual void Die() override;

	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;

	virtual AActor* GetCombatTarget_Implementation() override;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitDefaultAttribute() const override;

	void HitReactTagChanged(const FGameplayTag CallbackTag, const int32 NewCounter);

	virtual void PossessedBy(AController* NewController) override;
public:
	UPROPERTY(BlueprintReadOnly)
	bool bHightlighted{false};

	UPROPERTY(BlueprintReadOnly)
	bool bHitReacting{false};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Character Class Defaults")
	int32 Level{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults",DisplayName="职业类型")
	ECharacterClass CharacterClass{ ECharacterClass::Warrior };//敌人种类

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(EditAnywhere,Category="AI",DisplayName="行为树")
	TObjectPtr<UBehaviorTree>BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	UPROPERTY(BlueprintReadWrite, Category = "战斗")
	TObjectPtr<AActor> CombatTarget{ nullptr };//对战目标

};
