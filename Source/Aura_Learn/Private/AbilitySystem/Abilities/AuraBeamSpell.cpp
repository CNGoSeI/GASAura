// 学习使用

#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemBPLibary.h"
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

/**
 * 闪电击中的首个敌人周围的捕获
 * 排除技能发出者 和 首个敌人
 */
void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*>OverlappingActors;
	TArray<AActor*>ActorToIgnore;
	ActorToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorToIgnore.Add(MouseHitActor);

	UAuraAbilitySystemBPLibary::GetLivePlayersWithRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors, ActorToIgnore,
		MouseHitActor->GetActorLocation(),
		StartupAddtionalLen + GetAbilityLevel() * 50);

	//int32 NumAdditionalTargets = FMath::Min(MaxNumShockTargets, GetAbilityLevel()-1);
	int32 NumAdditionalTargets = 5;

	UAuraAbilitySystemBPLibary::GetClosesTarget(MouseHitActor->GetActorLocation(), NumAdditionalTargets, OverlappingActors, OutAdditionalTargets);
}
