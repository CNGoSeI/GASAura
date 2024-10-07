// 学习使用

#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

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

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OnwerCharacter)
	if (!OnwerCharacter->Implements<UCombatInterface>())return;
	if (auto WeapnCmpt = ICombatInterface::Execute_GetWeapon(OnwerCharacter))
	{
		TArray<AActor*>IgnorelActor;
		IgnorelActor.Add(OnwerCharacter);
		auto Begin=WeapnCmpt->GetSocketLocation(FName("TipSocket"));
		FHitResult HitRes;

		/*ETraceTypeQuery::TraceTypeQuery1 是可视性通道*/
		UKismetSystemLibrary::SphereTraceSingle(OnwerCharacter, Begin, BeamTargetLocation, 20.f,
		                                        ETraceTypeQuery::TraceTypeQuery1, false, IgnorelActor,
		                                        EDrawDebugTrace::None, HitRes, true);

		if(HitRes.bBlockingHit)
		{
			MouseHitLocation = HitRes.ImpactPoint;
			MouseHitActor = HitRes.GetActor();
		}
	}
}
