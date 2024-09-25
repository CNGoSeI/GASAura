#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWgtController.generated.h"

UCLASS()
class AURA_LEARN_API USpellMenuWgtController : public UAuraWidgetController
{
	GENERATED_BODY()
protected:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbackToDependencies() override;
};
