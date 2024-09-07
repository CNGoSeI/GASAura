
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

const FName BBKey_HitReacting{ "HitReacting" };
const FName BBKey_Dead{ "bDead" };
const FName BBKey_RangedAttacker{ "RangedAttack" };

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
	if(IsValid(AuraAIController))AuraAIController->GetBlackboardComponent()->SetValueAsBool(BBKey_Dead, true);//将受击初始为false
	SetLifeSpan(6.f);
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation()
{
	return CombatTarget;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();

	if(HasAuthority())
	{
		UAuraAbilitySystemBPLibary::GiveStartupAbilities(this, AbilitySystemComponent,CharacterClass);//该函数存在访问GameMode
	}

	//绑定属性变化时的代理，让转播代理进行广播
	if(auto AuraAS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
			[this, AuraAS](const FOnAttributeChangeData& Data)
			{
				OnHealthChangedDel.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChangedDel.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGmaeplayTags::GetInstance().EffectHitReact,
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject
		(this, &AAuraEnemy::HitReactTagChanged); //Tag增减时的回调

		//初始值广播	
		OnHealthChangedDel.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChangedDel.Broadcast(AuraAS->GetMaxHealth());
	}

	//控制层设置
	if (auto HelthWgt = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		HelthWgt->SetWidgetController(this);//控制层就是这个Pawn
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
	UAuraAbilitySystemBPLibary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, const int32 NewCounter)
{
	bHitReacting = NewCounter > 0;

	GetCharacterMovement()->StopMovementImmediately();
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(BBKey_HitReacting, bHitReacting);
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
		AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->GetBlackboardAsset());
		AuraAIController->RunBehaviorTree(BehaviorTree);

		AuraAIController->GetBlackboardComponent()->SetValueAsBool(BBKey_HitReacting, false);//将受击初始为false
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(BBKey_RangedAttacker, CharacterClass != ECharacterClass::Warrior);//不是战士则视为远程攻击者

	}
	
}
