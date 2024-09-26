#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer&/*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGivenDel)
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&)
DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityStatusChangeDel, const FGameplayTag&/*GATag*/, const FGameplayTag&/*StatusTag*/);

UCLASS()
class AURA_LEARN_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSeted();//AvatarActor和OwnerActor 设置完成之后的调用

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities);//被动技能添加,会激活一次

	void AbilityInputTagHeld(const FGameplayTag& InputTag);//查找到可以激活的GA 触发GA的按住函数 如果GA未在激活中 则激活GA
	void AbilityInputTagReleased(const FGameplayTag& InputTag);//查找可激活的GA 触发GA的释放操作函数

	void ForEachAbility(const FForEachAbility& Delegate);//使入参代理进行广播该ASC中每个有效的技能

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& GASpec);//从技能中找到目标的技能Tag
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& GASpec);//从技能中找到目标输入Tag
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& GASpec);//从技能获取解锁状态

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);//从技能Tag获取技能规格

	void UpgradeAttribute(const FGameplayTag& AttributeTag);//根据属性标签升级对应属性

	/**
	 * Server 此函数仅在服务器上执行
	 */
	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);//RPC 升级属性

	void UpdateAbilityStatus(const int32 Level);//更新技能的状态

protected:
	//被GE应用自身时的广播绑定
	UFUNCTION(Client,Reliable)//仅在拥有在其上调用此函数的对象的客户端上执行，函数命名存在套路；此函数将通过网络复制
	void ClinetEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle GameplayEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& GATag, const FGameplayTag& StatusTag);

	virtual void OnRep_ActivateAbilities() override;
public:
	FEffectAssetTags EffectAssetTagsDel;

	FAbilitiesGivenDel AbilitiesGiveDel;//技能添加时进行广播的代理

	FAbilityStatusChangeDel AbilityStatusChangeDel;//技能状态改变时的代理

	bool bStartupAbilitiesGiven{ false };//初始技能是否已经获取
};
