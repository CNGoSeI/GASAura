#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWgtController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedDel, const bool, bSpendPointBtnEnabled,const bool,bEquipBtnEnabaled,const FString&,DescriptionStr, const FString&,NextLevelDescription);

struct FSelectAbility
{
	FGameplayTag Ability{};
	FGameplayTag Status{};
};

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

	UFUNCTION(BlueprintCallable)
	void SpendPointBtnPressed();

protected:

	//根据技能状态和技能点数判断界面按钮启用状态
	static void ShouldEnableBtns(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointBtn, bool& bShouldEnbalEquipBtn);
	void UpdateAbilityWgtStatus(const FGameplayTag& GATag, const FGameplayTag& StatusTag, const int32 SpellPoint);//广播按钮启用状态和新的描述文本
private:

	//缓存上次选择的技能 如果其他地方导致技能状态改变，依照这个缓存 更新内容 进行广播通知界面按钮
	FSelectAbility SelectAbility{ FAuraGmaeplayTags::GetInstance().Abilities_None,FAuraGmaeplayTags::GetInstance().Abilities_Status_Locked };
	int32 CurrentSpellpoint{ 0 };

};
