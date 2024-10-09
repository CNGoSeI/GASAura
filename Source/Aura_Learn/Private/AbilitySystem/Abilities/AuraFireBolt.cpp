#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemBPLibary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	//������Ϸ�������ڷ�����
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)return;

	auto CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const auto SocketLocation = ICombatInterface::Execute_GetCombatSocktLocation
	(GetAvatarActorFromActorInfo(), FAuraGmaeplayTags::GetInstance().CombatSocket_Weapon);

	FRotator  Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();//��ȡ����λ�õ���ת��
	//��������µ�pitch��ת
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	
	const FVector Forward = Rotation.Vector();
	const FVector Left = Forward.RotateAngleAxis(-ProjectileSpread / 2.f, FVector::UpVector);
	const FVector Right = Forward.RotateAngleAxis(ProjectileSpread / 2.f, FVector::UpVector);

	int16 CurretnNum = FMath::Min(GetAbilityLevel(), NumProjectiles);

	const auto Rotators = UAuraAbilitySystemBPLibary::EvenlySpeacedRotators(Forward, FVector::UpVector, ProjectileSpread, CurretnNum);

	FTransform SpawnTransform;

	SpawnTransform.SetLocation(SocketLocation);

	for(const auto& Rotator:Rotators)
	{

		SpawnTransform.SetRotation(Rotator.Quaternion());

		//�������� ����finish �������
		auto ProjectfileActor = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectfileClass, SpawnTransform, GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		ProjectfileActor->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		if (HomingTarget&& HomingTarget->Implements<UCombatInterface>())
		{
			ProjectfileActor->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();//׷��Ŀ��Ϊ�������
		}
		else
		{
			ProjectfileActor->HomingTargetSceneCmpt = NewObject<USceneComponent>(USceneComponent::StaticClass());//��Ҫ����new�Ķ��󱻼���GC����˼����ڷ�����
			ProjectfileActor->HomingTargetSceneCmpt->SetWorldLocation(ProjectileTargetLocation);
			ProjectfileActor->ProjectileMovement->HomingTargetComponent = ProjectfileActor->HomingTargetSceneCmpt;
		}

		ProjectfileActor->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingMinSpeed,HomingMaxSpeed);//׷ѰĿ����ٶ�
		ProjectfileActor->ProjectileMovement->bIsHomingProjectile = bLaunchHoming;

		ProjectfileActor->FinishSpawning(SpawnTransform);
	}


}
