// 学习使用


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpwanLocation()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const float DeltaSpread = SpawnSpread / NumMinions;//计算每个生成物应该占用多少度的生成区域

	//扇形区域两边，由两个向量组成
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.F, FVector::UpVector); //绕向量旋转一定角度得到的向量
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.F, FVector::UpVector);

	TArray<FVector> SpawnLocations;
	for(int16 i{0};i<NumMinions;++i)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);//生成点位的向量
		const FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);//生成位置
		SpawnLocations.Emplace(ChosenSpawnLocation);
		DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 8.f, 8, FColor::White, false, 4.f);

		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location,
			Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.F);

		DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 8, FColor::Red, false, 3.f);
		DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 8, FColor::Red, false, 3.f);
	}

	return SpawnLocations;
}
