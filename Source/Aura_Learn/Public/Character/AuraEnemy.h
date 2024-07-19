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
class AURA_LEARN_API AAuraEnemy : public AAuraCharacterBase,public IEnemyInterface
{
	GENERATED_BODY()

public:
	virtual void HighlightActor() override;
	virtual void UnHightlightActor() override;

	UPROPERTY(BlueprintReadOnly)
	bool bHightlighted {false};
};
