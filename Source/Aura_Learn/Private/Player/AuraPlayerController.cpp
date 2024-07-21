// 学习使用


#include "Player/AuraPlayerController.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Player/AuraPlayerState.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;//允许复制 复制基本指的是当一个实体在服务器上发生变化时，该变化会被发送至每个客户端
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext)//断言 如果为false 直接崩溃
	auto SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(SubSystem);
	SubSystem->AddMappingContext(AuraContext,0);//优先级最高

	bShowMouseCursor = true;//显示鼠标光标
	DefaultMouseCursor = EMouseCursor::Default;

	//设置输入模式内容
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);//设置鼠标锁定到窗口的模式
	InputModeData.SetHideCursorDuringCapture(false);//当窗口捕捉到光标，不隐藏光标
	SetInputMode(InputModeData);

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//使用的增强输入，将输入组件获取
	auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);//该转换如果失败，触发断言

	//绑定输入动作到函数
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//控制器的旋转跟随玩家的视角，因此需要设置旋转和朝向
	const FRotator YawRotation {0,GetControlRotation().Yaw ,0};//根据控制器Yaw创建新的旋转
	//FRotationMatrix类是旋转构造的旋转矩阵。通过矩阵，可以将旋转应用于向量，以便在 3D 空间中将其旋转
	const FVector ForwardDirction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);//GetUnitAxis用于获取旋转矩阵对应轴的向量
	const FVector RightDirction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//在控制的pawn上应用输入移动
	if(auto ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirction, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirction, InputAxisVector.X);//输入中X是水平轴 AD
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit); //检测返回鼠标指针下的对象
	if (!CursorHit.bBlockingHit)return;

	static IEnemyInterface* CurrentActor{nullptr};
	static IEnemyInterface* LastActor{nullptr};
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor != nullptr)LastActor->UnHightlightActor();
	if (CurrentActor != nullptr)CurrentActor->HighlightActor();
	LastActor = CurrentActor;
}
