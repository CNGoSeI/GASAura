#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if(auto AuraASC=Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffectDel.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		if(AuraASC->bStartupAbilitiesGiven&& AuraASC->GetStatusFromAbilityTag(PassiveSpellTag)==FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped)
		{
			Activate();
		}
	}else
	{
		//���ûص��ķ�ʽ���ȴ�GAS��Чʱ�ٽ��а�
		if (auto CombatIF = Cast<ICombatInterface>(GetOwner()))
		{
			CombatIF->GetOnASCRegisteredDel().AddLambda([this](UAbilitySystemComponent* InAuraASC)
			{
				if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(
					UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
				{
					AuraASC->ActivatePassiveEffectDel.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);

					if (AuraASC->bStartupAbilitiesGiven && AuraASC->GetStatusFromAbilityTag(PassiveSpellTag) == FAuraGmaeplayTags::GetInstance().Abilities_Status_Equipped)
					{
						Activate();
					}
				}
			});
		}
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if(AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if(bActivate &&!IsActive())
		{
			Activate();
		}else
		{
			Deactivate();
		}

	}
}
