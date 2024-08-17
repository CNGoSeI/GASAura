// 学习使用

#include "AbilitySystem/AuraAbilitySystemBPLibary.h"

#include <memory>

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemBPLibary::GetOverlayWgtController(const UObject* WorldContenxt)
{
	auto WgtControllerParams = std::make_unique<FWidgetControllerParams>();
	bool bSucsses = SetWgtCtrlParamsByWorldContenxt(WorldContenxt, *WgtControllerParams);
	if (!bSucsses)return nullptr;
	auto AuraHUD = Cast<AAuraHUD>(WgtControllerParams->PlayerController->GetHUD());
	if (!IsValid(AuraHUD))return nullptr;

	return AuraHUD->GetOverlaiController(*WgtControllerParams);
}

UAttributeMenuWgtController* UAuraAbilitySystemBPLibary::GetAttributeMenuWgtController(const UObject* WorldContenxt)
{
	auto WgtControllerParams = std::make_unique<FWidgetControllerParams>();
	bool bSucsses=SetWgtCtrlParamsByWorldContenxt(WorldContenxt, *WgtControllerParams);
	if (!bSucsses)return nullptr;
	auto AuraHUD = Cast<AAuraHUD>(WgtControllerParams->PlayerController->GetHUD());
	if (!IsValid(AuraHUD))return nullptr;

	return AuraHUD->GetAtributeMenuController(*WgtControllerParams);
}

bool UAuraAbilitySystemBPLibary::SetWgtCtrlParamsByWorldContenxt(const UObject* WorldContenxt,
                                                                 OUT FWidgetControllerParams& WgtCtrlParam)
{
	auto Controller = UGameplayStatics::GetPlayerController(WorldContenxt, 0); //该函数在本地调用，0一定是玩家自己的控制器
	if (!IsValid(Controller))return false;

	auto PS = Controller->GetPlayerState<AAuraPlayerState>();
	if (!IsValid(PS))return false;

	auto ASC = PS->GetAbilitySystemComponent();
	auto AttributeSet = PS->GetAttributeSet();
	WgtCtrlParam.PlayerController = Controller;
	WgtCtrlParam.PlayerState = PS;
	WgtCtrlParam.AbilitySystemComponent = ASC;
	WgtCtrlParam.AttributeSet = AttributeSet;
	return true;
}

void UAuraAbilitySystemBPLibary::InitializeDefaultAttributes(const UObject* WorldContenxt, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	auto GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContenxt));
	if (!IsValid(GameMode))return;

	auto ClassDefaultInfo = GameMode->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	auto GEContext = ASC->MakeEffectContext();
	GEContext.AddSourceObject(ASC->GetAvatarActor());
	
	auto PrimaryAttributeSpecHandl = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, GEContext);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributeSpecHandl.Data.Get());

	auto SecondaryAttributeSpecHandl = ASC->MakeOutgoingSpec(GameMode->CharacterClassInfo->SecondaryAttributes, Level, GEContext);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributeSpecHandl.Data.Get());

	auto ViatalAttributeSpecHandl = ASC->MakeOutgoingSpec(GameMode->CharacterClassInfo->ViatalAttributes, Level, GEContext);
	ASC->ApplyGameplayEffectSpecToSelf(*ViatalAttributeSpecHandl.Data.Get());
}
