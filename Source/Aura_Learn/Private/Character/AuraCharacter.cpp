// 学习使用


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Misc/LowLevelTestAdapter.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	//相机相关
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate=FRotator( 0.f,400,0.f );//旋转至运动方向的速率
	GetCharacterMovement()->bConstrainToPlane = true;//运动将限制在一个平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//运动开始时与平面对齐

	//不使用控制器的旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//服务器初始化GAS的拥有者
	InitAbilityActorInfo();
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
}
