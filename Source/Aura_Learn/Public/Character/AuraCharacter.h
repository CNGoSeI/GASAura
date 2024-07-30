// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class AURA_LEARN_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();

	/*CombatInterface 实现区域*/
	virtual int32 GetPlyaerLevel() override;
	/*CombatInterface 结束区域*/

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

