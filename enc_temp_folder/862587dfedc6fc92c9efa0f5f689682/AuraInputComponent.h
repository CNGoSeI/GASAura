// 学习使用

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_LEARN_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	//将传入的函数与输入配置中的IA绑定
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig*, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
                                             ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	if(!IsValid(InputConfig))
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Is Null"))
		return;
	}

	for(const auto& InputAction: InputConfig->AbilityInputActions)
	{
		if(!IsValid(InputAction.InputAction)||!InputAction.InputTag.IsValid())continue;

		if(PressedFunc)
		{
			BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, std::forward<PressedFuncType>(PressedFunc), InputAction.InputTag);//最后tag 是因为该函数由可变参数结尾
		}

		if (ReleasedFunc)
		{
			BindAction(InputAction.InputAction, ETriggerEvent::Completed, Object, std::forward<ReleasedFuncType>(ReleasedFunc), InputAction.InputTag);
		}

		if (HeldFunc)
		{
			BindAction(InputAction.InputAction, ETriggerEvent::Triggered, Object, std::forward<HeldFuncType>(HeldFunc),InputAction.InputTag);
		}
		
	}
}
