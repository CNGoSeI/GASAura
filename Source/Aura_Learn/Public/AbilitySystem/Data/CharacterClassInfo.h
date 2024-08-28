﻿// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass:uint8
{
	Elementalist, //元素师
	Warrior, //战士
	Ranger, //游侠
};

USTRUCT()
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	
};

UCLASS()
class AURA_LEARN_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "普通角色类型属性")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "普通角色类型属性")
	TSubclassOf<UGameplayEffect> ViatalAttributes;

	UPROPERTY(EditDefaultsOnly,Category="默认角色类型属性")
	TMap<ECharacterClass, FCharacterClassDefaultInfo>CharacterClassInformation;

	UPROPERTY(EditDefaultsOnly, Category = "默认角色类型属性")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilyties;

	UPROPERTY(EditDefaultsOnly, Category = "默认角色类型属性|伤害",DisplayName="影响伤害系数表")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterType);


};
