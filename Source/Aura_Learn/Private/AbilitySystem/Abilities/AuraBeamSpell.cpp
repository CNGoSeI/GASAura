// 学习使用

#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if(HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOnwerVaribles()
{
	if(CurrentActorInfo)
	{
		OnwerPlayerController = CurrentActorInfo->PlayerController.Get();
		OnwerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
	
}
