// 学习使用

/*
 * 该类是项目角色基类 不允许创建该类实例 不允许tick 不需要控制器
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract)
class AURA_LEARN_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo(){};

	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& InitializeGEClass, float Level = 1.f) const;
	void InitDefaultAttribute() const;//根据GE初始化对应的属性

	void AddCharacterAbilities();
protected:
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon; //武器组件

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAtributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAtributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultVitalAtributes;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;//角色默认具备的技能
};
