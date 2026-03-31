// Copyright 2024. TEAM DAON. All rights reserved.

#include "Resource/HYBossField.h"

#include "Components/SphereComponent.h"
#include "Character/HYBossNPCBase.h"
#include "UI/HYBossHealthBar.h"
#include "Character/HYPlayerCharacterBase.h"
#include "DataAssets/HwanyoungType.h"

// Sets default values
AHYBossField::AHYBossField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AHYBossField::SphereBeginOverlap);
}

// Called when the game starts or when spawned
void AHYBossField::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(BossNPCRef))
	{
		UE_LOG(LogTemp, Error, TEXT("Valid Boss NPC Ref NoT FOUND"));
		return;
	}

	BossBarWG = CreateWidget<UHYBossHealthBar>(BossBarWGClass);
	BossBarWG->DamagableActor = BossNPCRef;
	BossBarWG->SetPercentage();
}

// Called every frame
void AHYBossField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHYBossField::SphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndexBody, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AHYPlayerCharacterBase>()
		&& OtherActor != BossNPCRef)
	{
		if (!IsValid(BossNPCRef))
		{
			return;
		}

		FBossType* BossData = DT_BossType->FindRow<FBossType>(BossNPCRef->BossNPCRowName, TEXT(""));

		if (BossData == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Valid Boss NPC REf NoT FOUND"));
			return;
		}

		BossBarWG->SetBossName(*BossData);

		BossNPCRef->OnHPChanged.AddDynamic(BossBarWG, &UHYBossHealthBar::SetPercentage);
		BossNPCRef->OnDeath.AddDynamic(this, &AHYBossField::Removeitself);
	}
}

void AHYBossField::SphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndexBody)
{
	if (OtherActor->IsA<AHYPlayerCharacterBase>())
	{
		BossBarWG->RemoveFromParent();
	}
}

void AHYBossField::Removeitself()
{
	BossBarWG->RemoveFromParent();

	Destroy();
}

