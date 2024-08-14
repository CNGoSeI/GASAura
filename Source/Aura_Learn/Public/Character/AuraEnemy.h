// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

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

	/*CombatInterface 实现区域*/
	FORCEINLINE virtual int32 GetPlyaerLevel() override;
	/*CombatInterface 结束区域*/

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

public:
	UPROPERTY(BlueprintReadOnly)
	bool bHightlighted{false};

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category="Character Class Defaults")
	int32 Level{ 1 };

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
};
