// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemBPLibary.generated.h"

class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
struct FWidgetControllerParams;
class UAttributeMenuWgtController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_LEARN_API UAuraAbilitySystemBPLibary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//为了能访问世界的某些内容，最好拿到世界上下文
	UFUNCTION(BlueprintPure,Category="AuraAbilitySystemLib|WgtController")
	static UOverlayWidgetController* GetOverlayWgtController(const UObject* WorldContenxt);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLib|WgtController")
	static UAttributeMenuWgtController* GetAttributeMenuWgtController(const UObject* WorldContenxt);

	static bool SetWgtCtrlParamsByWorldContenxt(const UObject* WorldContenxt,OUT FWidgetControllerParams& WgtCtrlParam);

	UFUNCTION(BlueprintCallable,Category="AuraAbilitySystemLib|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContenxt, ECharacterClass CharacterClass, float Level,UAbilitySystemComponent* ASC);//从CharacterClassInfo的配置获取信息初始化属性

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLib|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContenxt, UAbilitySystemComponent* ASC);//从CharacterClassInfo的配置获取初始的能力
};
