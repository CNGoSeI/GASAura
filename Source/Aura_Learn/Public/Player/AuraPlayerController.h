// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class IEnemyInterface;
class USplineComponent;
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
	friend class AAuraCharacterBase;

	AAuraPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;//PlayerTick只能用于本地Controller
private:
	void Move(const FInputActionValue& InputActionValue);
	void CursorTrace();

	void AbilityInputTagPressed(FGameplayTag InputTag); //按下的输入操作回调
	void AbilityInputTagReleased(FGameplayTag InputTag);//松开的输入操作回调
	void AbilityInputTagHeld(FGameplayTag InputTag);//按住的输入操作回调

	bool bShiftKeyDown{ false };//shift是否按住

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	
	void AutoRun();
	UAuraAbilitySystemComponent* GetAuraASC();

private:
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;//输入操作的DA 配置文件

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent{nullptr};

	IEnemyInterface* CurrentActor{ nullptr };//鼠标悬浮到的Actor

	FVector CachedDestination{FVector::Zero()};//缓存点中的场景坐标
	float FllowTime{0.f};//跟随光标移动了的时间
	float ShortPressThreshold{ 0.5f };//判断是否属于长按的临界值
	bool bAutoRunning{ false };
	bool bTargeting{ false };//鼠标点击是否存在可交互对象

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius{ 50.f };//距离目标多远则被视为到达目标

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;//根据点生成平滑的路径

	FHitResult CursorHit;
};
