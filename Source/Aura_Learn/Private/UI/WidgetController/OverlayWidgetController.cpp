// 学习使用


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	auto AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChangedDel.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChangedDel.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnMaxManaChangedDel.Broadcast(AuraAttributeSet->GetMaxMana());
	OnManaChangedDel.Broadcast(AuraAttributeSet->GetMana());
}

void UOverlayWidgetController::BindCallbackToDependencies()
{
	auto AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	BindAttributeChangeHelperFunc(AuraAttributeSet->GetHealthAttribute(), OnHealthChangedDel);
	BindAttributeChangeHelperFunc(AuraAttributeSet->GetMaxHealthAttribute(), OnMaxHealthChangedDel);
	BindAttributeChangeHelperFunc(AuraAttributeSet->GetManaAttribute(), OnManaChangedDel);
	BindAttributeChangeHelperFunc(AuraAttributeSet->GetMaxManaAttribute(), OnMaxManaChangedDel);

	if (auto AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		/**
		 * 因为时序上的问题，当ASC组件已经获取初始化技能时，不需要委托代理执行
		 * 但是但ASC 未添加完成初始化时，不应该直接调用OnInitStartupAbilities，等待ASC的代理调用即可
		 */
		if (AuraASC->bStartupAbilitiesGiven)
		{
			OnInitStartupAbilities(AuraASC);
		}
		else
		{
			AuraASC->AbilitiesGiveDel.AddUObject(this, &UOverlayWidgetController::OnInitStartupAbilities);
		}

		AuraASC->EffectAssetTagsDel.AddLambda([this](const FGameplayTagContainer& AssetTags)

		{
			for (auto& Tag : AssetTags)
			{
				auto MessageTag = FGameplayTag::RequestGameplayTag("Message");
				if (Tag.MatchesTag(MessageTag)) //TagA.MatchesTag(TagB) 判断A是否为B的子标签
				{
					auto Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					if (Row)MessageWidgetRowDel.Broadcast(*Row);
				}
			}
		});
	}
}

/*
 * 1.FOnAttributeChangedSignature 应该之后被改为一个代理通知变化 因此 适时再重启该函数
 * 2.使用方式如下 属性集目标 管理目标的代理
 *   - BindAttributeChangeHelperFunc （AttributeSet -> GetHealthAttribute （），OnHealthChanged ）； 
 */
void UOverlayWidgetController::BindAttributeChangeHelperFunc(FGameplayAttribute Attribute,
                                                             FOnAttributeChangedSignature& AttributeData)
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
		[this, &AttributeData](const FOnAttributeChangeData& Data)
		{
			AttributeData.Broadcast(Data.NewValue);
		});
}

void UOverlayWidgetController::OnInitStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
	if (!IsValid(AuraASC))return;

	FForEachAbility LoopBordGADel;
	LoopBordGADel.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		auto AbilityInfoTemp=AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		AbilityInfoTemp.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDel.Broadcast(AbilityInfoTemp);
	});

	AuraASC->ForEachAbility(LoopBordGADel);
}
