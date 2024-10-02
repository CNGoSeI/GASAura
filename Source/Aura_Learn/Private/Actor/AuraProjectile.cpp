﻿#include "Actor/AuraProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemBPLibary.h"
#include "Aura_Learn/Aura_Learn.h"
#include "Components/AudioComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;


	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);

	//仅查询 仅对 场景动态 场景静态 pawn 检测
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Sphere->SetCollisionObjectType(ECC_Projectile);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;//没有重力
}

void AAuraProjectile::Destroyed()
{
	if(!bHit&&!HasAuthority())//如果客户端没有应用击中产生的效果，那么此时需要应用
	{
		OnHit();
	}
	
	Super::Destroyed();
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	ProjectileLoopSoundCmpt =UGameplayStatics::SpawnSoundAttached(ProjectileSound, RootComponent);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlapPrimitiveComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	if (SourceAvatarActor == OtherActor)return;//仅客户端运行会打中自己

	if (!UAuraAbilitySystemBPLibary::IsNotFriend(OtherActor, SourceAvatarActor, "Enemy"))return;

	if(!bHit)//防止服务端已经击中，生成音效 复制给客户端时 客户端还生成音效
	{
		OnHit();
	}

	if(HasAuthority())
	{
		if(auto TargetAsc=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetAbilitySystemComponent = TargetAsc;

			UAuraAbilitySystemBPLibary::ApplyDamageEffect(DamageEffectParams);
		}

		Destroy();//服务器上销毁这个

	}else
	{
		bHit = true;//在服务端销毁前 客户端 可能还没有触发到这个事件， 因此标记一下已经完成触发，后续不再触发 
	}
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (IsValid(ProjectileLoopSoundCmpt))ProjectileLoopSoundCmpt->Stop();
	bHit = true;
}

