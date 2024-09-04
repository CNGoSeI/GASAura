﻿// 学习使用

#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura_Learn/Aura_Learn.h"
#include "Components/CapsuleComponent.h"
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
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FVector AAuraCharacterBase::GetCombatSocktLocation_Implementation(const FGameplayTag& AttackMontageTag)
{
	if (AttackMontageTag.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Montage_Attack_Weapon))
	{
		if (IsValid(Weapon))
		{
			return Weapon->GetSocketLocation(WeaponTipSocketName);
		}
	}
	else if (AttackMontageTag.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Montage_Attack_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	else if (AttackMontageTag.MatchesTagExact(FAuraGmaeplayTags::GetInstance().Montage_Attack_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandTipSocketName);
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
void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));//分离武器

	MulticastHandleDeath();
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation() const
{
	return AttackMontages;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{

	//开启布娃娃
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);//仅物理通道碰撞

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();

	bDead = true;
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
