#pragma once
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;
USTRUCT(BlueprintType)
struct FDamageGEParamsByDamageType
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	float DebuffFrequency = 1.f;//ִ��Ƶ��

	UPROPERTY()
	float DebuffDamage = 0.f;

	float BaseDamage = 0.f;

	float DebuffChance = 0.f;
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
	bool IsSuccessfulDebuff() const { return bIsSucessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }

	void SetCriticalHit(const bool bValue) { bIsCriticalHit = bValue; };
	void SetBlockedHit(const bool bValue) { bIsBlockedHit = bValue; };
	void SetIsSuccessfulDebuff(bool bInIsDebuff) { bIsSucessfulDebuff = bInIsDebuff; }
	void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage; }
	void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; }
	void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; }
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }

protected:
	UPROPERTY()
	bool bIsCriticalHit{ false };//�Ƿ񱩻�
	UPROPERTY()
	bool bIsBlockedHit{ false };//�Ƿ��

	UPROPERTY()
	bool bIsSucessfulDebuff{ false };//�ɹ�debuff?

	UPROPERTY()
	float DebuffDamage{ 0.f };

	UPROPERTY()
	float DebuffDuration{ 0.f };

	UPROPERTY()
	float DebuffFrequency{ 0.f };//ִ��Ƶ��


	TSharedPtr<FGameplayTag> DamageType;//�˺�����

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