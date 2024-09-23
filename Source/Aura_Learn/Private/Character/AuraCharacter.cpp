#include "Character/AuraCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "NiagaraComponent.h"

AAuraCharacter::AAuraCharacter()
{
	//相机相关
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true);
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;

	//升级特效
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagara");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	//运动系统调整
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate=FRotator( 0.f,400,0.f );//旋转至运动方向的速率
	GetCharacterMovement()->bConstrainToPlane = true;//运动将限制在一个平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//运动开始时与平面对齐

	//不使用控制器的旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	auto AutaPlayerState = CastChecked<AAuraPlayerState>(GetPlayerState());
	check(AutaPlayerState)
	return AutaPlayerState->GetPlayerLevel();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(1);

	MulticastLevelUpParticles();
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerXP();
}

int32 AAuraCharacter::FindLevelFromXP_Implementation(int32 XP) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->DA_LevelInfo->FindLevelForXP(XP);
}

int32 AAuraCharacter::GetAttributePointReward_Implementation(const int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->DA_LevelInfo->LevelUpInformation[Level - 1].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointReward_Implementation(const int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->DA_LevelInfo->LevelUpInformation[Level - 1].SpellPointAward;;
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InPlayerLevel);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoint) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoint);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoint) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoint);
}

int32 AAuraCharacter::GetCurrentAttributePoint_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetCurrentSpellPoint_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//服务器初始化GAS的拥有者
	InitAbilityActorInfo();

	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//客户端初始化GAS的拥有者
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	auto AutaPlayerState = CastChecked<AAuraPlayerState>(GetPlayerState());
	check(AutaPlayerState)
	AbilitySystemComponent = AutaPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AutaPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSeted();
	AttributeSet = AutaPlayerState->GetAttributeSet();

	//服务器拥有所有控制器 但是玩家客户端只有自己的 因此存在获取不到的的情况
	if (auto AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (auto AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AutaPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	InitDefaultAttribute();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if(IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation= Camera->GetComponentLocation();
		const FVector NiagaraComponentLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const auto ToCameraRotation=(CameraLocation - NiagaraComponentLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}
