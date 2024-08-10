// 学习使用


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//单人游戏或者是在服务器
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer)return;

	auto CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if(!CombatInterface) return;
	const auto SocketLocation = CombatInterface->GetCombatSocktLocation();
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);

	//延期生成 调用finish 完成生成
	auto Projectile=GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectfileClass, SpawnTransform, GetOwningActorFromActorInfo(),
	                                                 Cast<APawn>(GetOwningActorFromActorInfo()),
	                                                 ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	//TODO::给生成的技能一个GE 让被击中者应用

	Projectile->FinishSpawning(SpawnTransform);
}
