#pragma once
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageGEParamsByDamageType
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffDamage = 0.f;

	float BaseDamage = 0.f;
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams() {}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	UPROPERTY()
	float AbilityLevel = 1.f;

	UPROPERTY()
	TMap<FGameplayTag, FDamageGEParamsByDamageType> DebuffMapGEParams;//DeBuff���Ͷ�Ӧ��GE�������
};

USTRUCT(BlueprintType)
struct FAuraGameEffectContext :public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); };
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;//�������л�

	virtual FAuraGameEffectContext* Duplicate() const override
	{
		FAuraGameEffectContext* NewContext = new FAuraGameEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	};

	bool IsCriticalHit() const { return bIsCriticalHit; };
	bool IsBlockedHit() const { return bIsBlockedHit; };
	void SetCriticalHit(const bool bValue) { bIsCriticalHit = bValue; };
	void SetBlockedHit(const bool bValue) { bIsBlockedHit = bValue; };

protected:
	UPROPERTY()
	bool bIsCriticalHit{ false };//�Ƿ񱩻�
	UPROPERTY()
	bool bIsBlockedHit{ false };//�Ƿ��
private:

};

template<>
struct  TStructOpsTypeTraits<FAuraGameEffectContext>:TStructOpsTypeTraitsBase2<FAuraGameEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// �Ա㽫 TSharedPtr<FHitResult> ���ݸ��Ƶ�����
	};

};