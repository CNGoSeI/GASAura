#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class AURA_LEARN_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();

	/*CombatInterface 实现区域*/
	virtual int32 GetPlayerLevel_Implementation() override;
	/*CombatInterface 结束区域*/

	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;//当 PlayerState 属性在服务器上复制到客户端时，这个函数会被调用
private:
	virtual void InitAbilityActorInfo() override;
protected:
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent>SpringArm;
	UPROPERTY(EditAnywhere,Category="Camera")
	TObjectPtr<UCameraComponent>Camera;
};

