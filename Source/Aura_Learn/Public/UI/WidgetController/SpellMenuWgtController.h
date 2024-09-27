#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWgtController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellGlobeSelectedDel, const bool, bSpendPointBtnEnabled,const bool,bEquipBtnEnabaled);

UCLASS(BlueprintType,Blueprintable)
class AURA_LEARN_API USpellMenuWgtController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbackToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSignature SpellPointChangeDel;

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);//界面中的技能球被选中时触发

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedDel SpellGlobeSelectedDel;

protected:

	//根据技能状态和技能点数判断界面按钮启用状态
	static void ShouldEnableBtns(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointBtn, bool& bShouldEnbalEquipBtn);

};
