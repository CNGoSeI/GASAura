// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct  FInputActionValue;
/**
 * 
 */
UCLASS()
class AURA_LEARN_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;//仅限控制器有输入时才Tick
private:
	void Move(const FInputActionValue& InputActionValue);
	void CursorTrace();

	void AbilityInputTagPressed(FGameplayTag InputTag); //按下的输入操作回调
	void AbilityInputTagReleased(FGameplayTag InputTag);//松开的输入操作回调
	void AbilityInputTagHeld(FGameplayTag InputTag);//按住的输入操作回调

	UAuraAbilitySystemComponent* GetAuraASC();

private:
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;//输入操作的DA 配置文件

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent{nullptr};
};
