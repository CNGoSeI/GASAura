﻿// 学习使用

#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Aura_Learn/Aura_Learn.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false; 
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));

	//武器设置
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));//武器绑定到武器插槽
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭武器碰撞

	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);//生成重叠碰撞检测事件

	//debuff组件
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(RootComponent);
	BurnDebuffComponent->DebuffTag = FAuraGmaeplayTags::GetInstance().Debuff_Burn;

}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FVector AAuraCharacterBase::GetCombatSocktLocation_Implementation(const FGameplayTag& AttackMontageTag)
{
	if (AttackMontageTag.MatchesTagExact(FAuraGmaeplayTags::GetInstance().CombatSocket_Weapon))
	{
		if (IsValid(Weapon))
		{
			return Weapon->GetSocketLocation(WeaponTipSocketName);
		}
	}
	else if (AttackMontageTag.MatchesTagExact(FAuraGmaeplayTags::GetInstance().CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	else if (AttackMontageTag.MatchesTagExact(FAuraGmaeplayTags::GetInstance().CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandTipSocketName);
	}
	else if(AttackMontageTag.MatchesTagExact(FAuraGmaeplayTags::GetInstance().CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

FHitResult* AAuraCharacterBase::GetCursorHitRes()
{
	return &(Cast<AAuraPlayerController>(GetController())->CursorHit);
}

UAnimMontage* AAuraCharacterBase::GetHitRecatMontag_Implementation() const
{
	return HitReactMontage;
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation() const
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for(const auto& it:AttackMontages)
	{
		if (it.MontageTag == MontageTag)return it;
	}
	return FTaggedMontage{};
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

int32 AAuraCharacterBase::IncrementMinionCount_Implementation(int32 DeltaCount)
{
	MinionCount += DeltaCount;
	return MinionCount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClassType_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered AAuraCharacterBase::GetOnASCRegisteredDel()
{
	return OnASCRegistered;
}

FOnDeathDel& AAuraCharacterBase::GetOnDeathDel()
{
	return OnSelfDead;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));//分离武器

	MulticastHandleDeath(DeathImpulse);
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation() const
{
	return AttackMontages;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(),GetActorRotation());

	//开启布娃娃
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);//仅物理通道碰撞
	Weapon->AddImpulse(DeathImpulse*0.1, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse,NAME_None,true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();

	bDead = true;
	OnSelfDead.Broadcast(this);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& InitializeGEClass, float Level) const
{
	if (!IsValid(InitializeGEClass) || !IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("DefaultSecondaryAtributes Not Setup Form AAuraCharacterBase!!!"))
		return;
	}
	auto InitGESpec = AbilitySystemComponent->MakeEffectContext();
	InitGESpec.AddSourceObject(this);
	auto InitGEHandle = AbilitySystemComponent->MakeOutgoingSpec(InitializeGEClass, Level, InitGESpec);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*InitGEHandle.Data.Get());
}

void AAuraCharacterBase::InitDefaultAttribute() const
{
	ApplyEffectToSelf(DefaultPrimaryAtributes);
	ApplyEffectToSelf(DefaultSecondaryAtributes);
	ApplyEffectToSelf(DefaultVitalAtributes);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	auto GAS = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	if ((!HasAuthority())||!IsValid(GAS))return; //Actor是否有网络权限（就是是否为服务端的实例）
	GAS->AddCharacterAbilities(StartupAbilities);
	GAS->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	UMaterialInstanceDynamic* DynamicMatInst{ nullptr };
	if(IsValid(DissolveMaterialOfMesh))
	{
		DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialOfMesh, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimlineOfMesh(DynamicMatInst);
	}
	if(IsValid(DissolveMaterialOfWeapon))
	{
		DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialOfWeapon, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartDissolveTimlineOfWeapon(DynamicMatInst);
	}
}
