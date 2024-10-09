#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
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

	/*PlayerInterface*/
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelFromXP_Implementation(int32 XP) const override;
	virtual int32 GetAttributePointReward_Implementation(const int32 Level) const override;
	virtual int32 GetSpellPointReward_Implementation(const int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) const override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoint) const override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoint) const override;
	virtual int32 GetCurrentAttributePoint_Implementation() const override;
	virtual int32 GetCurrentSpellPoint_Implementation() const override;
	/*PlayerInterface*/

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;//当 PlayerState 属性在服务器上复制到客户端时，这个函数会被调用
	virtual void OnRep_Stunned() override;
private:
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelUpParticles()const;//向其他客户端广播升级特效
protected:
	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent>SpringArm;
	UPROPERTY(EditAnywhere,Category="Camera")
	TObjectPtr<UCameraComponent>Camera;

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
};

