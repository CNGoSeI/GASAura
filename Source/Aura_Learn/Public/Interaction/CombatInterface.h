#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;
/**
 * 用以给蒙太奇带个标签，方便从标签识别不同类型进行分别处理u'ra
 */
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,DisplayName="攻击蒙太奇")
	UAnimMontage* Montage{ nullptr }; //攻击动画

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,DisplayName="攻击类型标签")
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName = "插槽类型标签")
	FGameplayTag SocketTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,DisplayName="受击音效")
	USoundBase* ImpactSound{ nullptr };
};

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

	virtual FHitResult* GetCursorHitRes();//鼠标指针的检测反射获取

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetFaceingTargetLoc(const FVector& FaceingLoction);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitRecatMontag() const;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	AActor* GetCombatTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCombatSocktLocation(const FGameplayTag& AttackMontageTag);//获取武器发射物品的插槽位置

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetAvatar();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FTaggedMontage> GetAttackMontages() const;

	virtual void Die()=0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect()const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount();//返回场景中召唤物的数量

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 IncrementMinionCount(int32 DeltaCount);//调整召唤物的数量

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClassType();//获取角色的职业类型
};
