// 学习使用


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	auto AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
}

void UOverlayWidgetController::BindCallbackToDependencies()
{
	auto AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

 //将对应属性变化 发出的广播 绑定转发
#define BIND_CALL_BACK( AttributeName) AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(\
	AuraAttributeSet->Get##AttributeName##Attribute()).AddLambda( [this](const FOnAttributeChangeData& Data)\
    {\
	On##AttributeName##Changed.Broadcast(Data.NewValue);\
	});
	BIND_CALL_BACK(Health);
	BIND_CALL_BACK(MaxHealth);
	BIND_CALL_BACK(MaxMana);
	BIND_CALL_BACK(Mana);
#undef BIND_CALL_BACKS  //#undef 是取消宏定义  主要是表示define 结束 但是不想去掉def 因此加个s

}

/*
 * 1.FOnAttributeChangedSignature 应该之后被改为一个代理通知变化 因此 适时再重启该函数
 * 2.使用方式如下 属性集目标 管理目标的代理
 *   - BindAttributeChange （AttributeSet -> GetHealthAttribute （），OnHealthChanged ）； 
 */
//void UOverlayWidgetController::BindAttributeChange(FGameplayAttribute Attribute, FOnAttributeChangedSignature& AttributeData)
//{
//	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
//		[this, &AttributeData](const FOnAttributeChangeData& Data)
//		{
//			AttributeData.Broadcast(Data.NewValue);
//		});
//}
