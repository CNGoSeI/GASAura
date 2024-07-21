// 学习使用


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(auto ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		//StaticClass获取类的元信息对象（UClass 对象）。每个从 UObject 继承的类都会自动生成这个函数。
		const auto asc = ASCInterface->GetAbilitySystemComponent();
		auto AttributeSet=Cast<UAuraAttributeSet>(asc->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		auto AuraAttributeSet = const_cast<UAuraAttributeSet*>(AttributeSet);//暂时这样，后面必定GE
		AuraAttributeSet->SetHealth(AttributeSet->GetHealth()+25.f);//回血25
		Destroy();
	}
}

void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::EndOverlap);
}

