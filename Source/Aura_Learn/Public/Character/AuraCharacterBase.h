﻿// 学习使用

/*
 * 该类是项目角色基类 不允许创建该类实例 不允许tick 不需要控制器
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract)
class AURA_LEARN_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };
	virtual FVector GetCombatSocktLocation_Implementation(const FGameplayTag& AttackMontageTag) override;
	virtual FHitResult* GetCursorHitRes() override;
	virtual UAnimMontage* GetHitRecatMontag_Implementation() const override; //获取受击动画
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual int32 IncrementMinionCount_Implementation(int32 DeltaCount) override;
	virtual ECharacterClass GetCharacterClassType_Implementation() override;
	/*
	 *  NetMulticast 此函数将在服务器上本地执行，也将复制到所有客户端上，无论该Actor的 NetOwner 为何。
	 *  此函数将通过网络复制，并且一定会到达，即使出现带宽或网络错误。
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(); //死亡时进行

	virtual void Die() override;

	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;

	UPROPERTY(EditAnywhere,Category="Combat")
	TArray<FTaggedMontage> AttackMontages;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo()
	{
	};

	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& InitializeGEClass, float Level = 1.f) const;
	virtual void InitDefaultAttribute() const; //根据GE初始化对应的属性

	void AddCharacterAbilities();

	/* 溶解材质 */

	void Dissolve();//将材质替换为溶解材质

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimlineOfMesh(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimlineOfWeapon(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UPROPERTY(DisplayName="角色溶解材质", EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialOfMesh{nullptr};
	UPROPERTY(DisplayName = "武器溶解材质", EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialOfWeapon{ nullptr };

	int16 MinionCount {0};//召唤物的数量

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon; //武器组件

	UPROPERTY(EditAnywhere, Category = "Combat", DisplayName = "武器判定插槽")
	FName WeaponTipSocketName; //定位生成技能的插槽名字

	UPROPERTY(EditAnywhere, Category = "Combat",DisplayName="左手判定插槽")
	FName LeftHandSocketName; 

	UPROPERTY(EditAnywhere, Category = "Combat", DisplayName = "右手判定插槽")
	FName RightHandTipSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat", DisplayName = "尾部判定插槽")
	FName TailSocketName;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities; //角色默认具备的技能

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities; //角色默认具备的被动技能

	UPROPERTY(EditDefaultsOnly, Category = "CombatPower")
	TObjectPtr<UAnimMontage> HitReactMontage{nullptr};

	bool bDead{ false };

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat",DisplayName="贱血特效")
	UNiagaraSystem* BloodEffect; //贱血效果

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", DisplayName = "死亡音效")
	USoundBase* DeathSound;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults", DisplayName = "职业类型")
	ECharacterClass CharacterClass{ ECharacterClass::Warrior };//敌人种类
};
