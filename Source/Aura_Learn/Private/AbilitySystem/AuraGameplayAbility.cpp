﻿// 学习使用


#include "AbilitySystem/AuraGameplayAbility.h"

FString UAuraGameplayAbility::GetDescription(const int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"默认技能名字 - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(const int32 Level)
{
	return FString::Printf(TEXT("<Default>下一级: </><Level>%d</> \n<Default>造成更多伤害 </>"), Level);
}
