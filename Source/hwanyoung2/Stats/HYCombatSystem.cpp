// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYCombatSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/HYPlayerCharacterBase.h"
#include "Character/HYAggroNPCBase.h"

// Sets default values for this component's properties
UHYCombatSystem::UHYCombatSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHYCombatSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHYCombatSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (true == bWeaponTrace)
	{
		PerformMultiTrace();
	}
}

bool UHYCombatSystem::PerformMultiTrace()
{
	bool bHasTakenDamage = false;

	if (IsValid(Mesh))
	{
		FVector CurrentTopTrace = Mesh->GetSocketLocation(TEXT("TopTrace"));
		FVector CurrentBottomTrace = Mesh->GetSocketLocation(TEXT("BottomTrace"));

		TArray<FHitResult> OutHits;
		float fHalfSizeX = UKismetMathLibrary::Vector_Distance(CurrentTopTrace, CurrentBottomTrace) / 2.0f;
		FRotator Orientation = UKismetMathLibrary::FindLookAtRotation(CurrentTopTrace, CurrentBottomTrace);
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectType = { EObjectTypeQuery::ObjectTypeQuery3 };

		// Box trace for getting hit actors
		bool bHitResult = UKismetSystemLibrary::BoxTraceMultiForObjects(this, PreviousTopSocket, CurrentTopTrace,
			FVector(fHalfSizeX, 15.0f, 30.0f), Orientation, ObjectType, false,
			TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true);

		// update the location of sockets
		PreviousTopSocket = CurrentTopTrace;
		PreviousBottomSocket = CurrentBottomTrace;

		for (auto OutHit : OutHits)
		{
			// if hit success
			if (true == OutHit.bBlockingHit)
			{
				AActor* HitActor = OutHit.GetActor();
				// If the hit actor is the first identified actor
				if (false == AlreadyDamaged.Contains(HitActor))
				{
					if (Cast<AHYPlayerCharacterBase>(HitActor))
					{
						AlreadyDamaged.Add(HitActor);
						OnTraceDamage.Broadcast(HitActor, OutHit);
						return true;
					}
				}
			}
		}
	}

	return bHasTakenDamage;
}

void UHYCombatSystem::ToggleWeaponTrace(bool _WeaponTraceOn)
{
	bWeaponTrace = _WeaponTraceOn;

	if (bWeaponTrace)
	{
		if (IsValid(Mesh) == false)
		{
			Mesh = Cast<AHYAggroNPCBase>(GetOwner())->GetCharacterMesh();
		}
		
		PreviousTopSocket = Mesh->GetSocketLocation(TEXT("TopTrace"));
		PreviousBottomSocket = Mesh->GetSocketLocation(TEXT("BottomTrace"));
	}
	else
	{
		AlreadyDamaged.Empty();
	}
}

