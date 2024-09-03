// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UAnimMontage;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_LEARN_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPlayerLevel();
	virtual FVector GetCombatSocktLocation();//获取武器发射物品的插槽位置
	virtual FHitResult* GetCursorHitRes();//鼠标指针的检测反射获取

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetFaceingTargetLoc(const FVector& FaceingLoction);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitRecatMontag() const;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	AActor* GetCombatTarget();

	virtual void Die()=0;
};
