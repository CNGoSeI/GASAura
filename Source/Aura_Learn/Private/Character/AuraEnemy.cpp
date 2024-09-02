// 学习使用


#include "Character/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemBPLibary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura_Learn/Aura_Learn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);//组件类的启用网络复制
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAuraEnemy::HighlightActor()
{
	bHightlighted = true;
	//高亮物体是通过后期处理材质，并且设置网格体渲染深度模板实现
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(Aura::CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(Aura::CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHightlightActor()
{
	bHightlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	GetMesh()->SetCustomDepthStencilValue(0);
	Weapon->SetRenderCustomDepth(false);
	Weapon->SetCustomDepthStencilValue(0);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy::Die()
{
	Super::Die();
	SetLifeSpan(6.f);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();

	if(HasAuthority())
	{
		UAuraAbilitySystemBPLibary::GiveStartupAbilities(this, AbilitySystemComponent);//该函数存在访问GameMode
	}

	//控制层设置
	if(auto HelthWgt=Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		HelthWgt->SetWidgetController(this);//控制层就是这个Pawn
	}


	//绑定属性变化时的代理，让转播代理进行广播
	if(auto AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGmaeplayTags::GetInstance().EffectHitReact,
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject
		(this, &AAuraEnemy::HitReactTagChanged); //Tag增减时的回调

		//初始值广播	
		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
	}

}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSeted();

	if(HasAuthority())
	{
		InitDefaultAttribute();
	}
	
	//ApplyEffectToSelf(DefaultPrimaryAtributes);
}

void AAuraEnemy::InitDefaultAttribute() const
{
	UAuraAbilitySystemBPLibary::InitializeDefaultAttributes(this, ChracterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, const int32 NewCounter)
{
	bHitReacting = NewCounter > 0;

	if(bHitReacting)
	{
		GetCharacterMovement()->StopMovementImmediately();
	}

}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority())return;//AI控制器不会存在客户端，不然客户端获取必然是无效的
	AuraAIController = Cast<AAuraAIController>(NewController);
	if (!IsValid(AuraAIController))return;

	if(IsValid(BehaviorTree))
	{
		AuraAIController->RunBehaviorTree(BehaviorTree);
		AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->GetBlackboardAsset());
	}
	
}
