#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_LEARN_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly,Category="Input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(const int32 Level);//获取技能的文本描述
	virtual FString GetNextLevelDescription(const int32 Level);
	static FString GetLockedDescription(const int32 Level);
};

inline FString UAuraGameplayAbility::GetLockedDescription(const int32 Level)
{
	return FString::Printf(TEXT("<Default> %d 级解锁技能</>"), Level);
}
