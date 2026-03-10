// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYDamageAmount.h"

#include "DataAssets/CombatTypes.h"
#include "Components/WidgetComponent.h"
#include "System/HYPoolSubSystem.h"

// Sets default values
AHYDamageAmount::AHYDamageAmount()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	RootComponent = DamageWidget;
}

// Called when the game starts or when spawned
void AHYDamageAmount::BeginPlay()
{
	Super::BeginPlay();
	
	DamageAmountWidget = DamageWidget->GetWidget();

	if (MoveTimelineCurve)
	{
		FOnTimelineFloat MoveFloat;
		MoveFloat.BindDynamic(this, &AHYDamageAmount::OnDamageAmountMoveUpdate);
		MoveTimeline.AddInterpFloat(MoveTimelineCurve, MoveFloat);

		FOnTimelineEvent MoveEvent;
		MoveEvent.BindDynamic(this, &AHYDamageAmount::OnDamageAmountMoveEnd);
		MoveTimeline.SetTimelineFinishedFunc(MoveEvent);

		MoveTimeline.SetLooping(false);
	}
}

void AHYDamageAmount::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveTimeline.IsPlaying())
	{
		MoveTimeline.TickTimeline(DeltaTime);
	}
}

void AHYDamageAmount::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool_Implementation();

	// Initialize damage amount and type of the widget.
	DamageAmountWidget->SetDamageAmount(DamageAmount);
	DamageAmountWidget->SetDamageType(DamageType);
	StartLocation = GetActorLocation();

	EndLocation = StartLocation + FVector(
		FMath::RandRange(-25.0f, 25.0f), FMath::RandRange(-25.0f, 25.0f), FMath::RandRange(35.0f, 70.0f));

	MoveTimeline.PlayFromStart();
}

void AHYDamageAmount::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool();
}

void AHYDamageAmount::OnDamageAmountMoveUpdate(float MoveTrack)
{
	SetActorLocation(FMath::Lerp(StartLocation, EndLocation, MoveTrack));
}

void AHYDamageAmount::OnDamageAmountMoveEnd()
{
	auto PoolSystem = GetGameInstance()->GetSubsystem<UHYPoolSubSystem>();

	PoolSystem->ReturnToPool(this);
}

void AHYDamageAmount::Initialize(float inDamageAmount, EDamageType inDamageType)
{
	DamageAmount = inDamageAmount;
	DamageType = inDamageType;
}


