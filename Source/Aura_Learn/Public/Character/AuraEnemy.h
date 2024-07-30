// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

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

};
