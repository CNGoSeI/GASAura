// 学习使用


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


}

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	//单人游戏或者是在服务器
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)return;

	auto CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const auto SocketLocation = ICombatInterface::Execute_GetCombatSocktLocation
		(GetAvatarActorFromActorInfo(), FAuraGmaeplayTags::GetInstance().Montage_Attack_Weapon);

	FRotator  Rotation = (TargetLocation - SocketLocation).Rotation();//获取两个位置的旋转差

	//Rotation += GetAvatarActorFromActorInfo()->GetActorRotation();
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	//延期生成 调用finish 完成生成
	auto ProjectfileActor = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectfileClass, SpawnTransform, GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	//给生成的技能一个GE 让被击中者应用
	auto SourceASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

	//装载GE上下文
	auto EffectContenxtHandle = SourceASC->MakeEffectContext();
	EffectContenxtHandle.SetAbility(this);
	EffectContenxtHandle.AddSourceObject(ProjectfileActor);
	FHitResult HitResult;
	HitResult.Location = TargetLocation;
	EffectContenxtHandle.AddHitResult(HitResult);

	auto DamageGESpecHandle=SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContenxtHandle);

	auto GameplayTags = FAuraGmaeplayTags::GetInstance();

	for(auto& it:DamageTypes)
	{
		const float ScaledDamage = it.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageGESpecHandle, it.Key, ScaledDamage);
	}
	
	
	ProjectfileActor->DamageEffectSpecHandle = DamageGESpecHandle;

	ProjectfileActor->FinishSpawning(SpawnTransform);
}
