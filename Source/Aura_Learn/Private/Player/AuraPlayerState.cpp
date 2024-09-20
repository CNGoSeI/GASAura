// 学习使用


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); //组件类的启用网络复制
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);//设置同步模式


	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f; //设置服务端将此对象每秒更新的次数
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

int16 AAuraPlayerState::GetPlayerLevel() const
{
	return Level;
}

int32 AAuraPlayerState::GetPlayerXP() const
{
	return XP;
}

void AAuraPlayerState::SetXP(const int32 InXP)
{
	XP = InXP;
	OnXPChangeDel.Broadcast(XP);
}

void AAuraPlayerState::AddToXP(const int32 InXP)
{
	XP += InXP;
	OnXPChangeDel.Broadcast(XP);
}

void AAuraPlayerState::SetLevel(const int32 InLevel)
{
	Level = InLevel;
	OnLevelChangeDel.Broadcast(Level);
}

void AAuraPlayerState::AddToLevel(const int32 InLevel)
{
	Level += InLevel;
	OnLevelChangeDel.Broadcast(Level);
}

void AAuraPlayerState::OnRep_Level(int16 OldValue)
{
	OnLevelChangeDel.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldValue)
{
	OnXPChangeDel.Broadcast(XP);
}
