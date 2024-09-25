#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWgtController.generated.h"

UCLASS(BlueprintType,Blueprintable)
class AURA_LEARN_API USpellMenuWgtController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbackToDependencies() override;

protected:

};
