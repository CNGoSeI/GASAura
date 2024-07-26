// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "AuraAttributeSet.generated.h"

#define \
ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//用以储存GAS 相关的组件内容
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {};

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC{ nullptr };
	UPROPERTY()
	TObjectPtr<AActor> SourceAvatarActor{ nullptr };
	UPROPERTY()
	TObjectPtr<AController>SourceController{nullptr};
	UPROPERTY()
	TObjectPtr<ACharacter>SourceCharacter{nullptr};

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC{ nullptr };
	UPROPERTY()
	TObjectPtr<AActor> TargetAvatarActor{ nullptr };
	UPROPERTY()
	TObjectPtr<AController>TargetController{ nullptr };
	UPROPERTY()
	TObjectPtr<ACharacter>TargetCharacter{ nullptr };

};

UCLASS()
class AURA_LEARN_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();

	//指定哪些属性需要在服务器和客户端之间同步
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;//Current Value被改变前调用
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;//Base Value改变后调用
	/*服务器回调区域*/
	UFUNCTION() //该回调必须UFUNCTION
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const; //带参数就会将旧的值传回
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue) const;
	/*服务器回调区域*/

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props);//将拿到的数据应用上FEffectProperties中
public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category="Vital Attributes")//ReplicatedUsing Specifier 指定了一个回调函数，该函数在属性通过网络更新时执行。
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana)
};
