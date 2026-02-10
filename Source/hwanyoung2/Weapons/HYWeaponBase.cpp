// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYWeaponBase.h"
#include "Stats/HYAttributeSystem.h"

// Sets default values
AHYWeaponBase::AHYWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	AttributeSystem = CreateDefaultSubobject<UHYAttributeSystem>(TEXT("AC_AttributeSystem"));
}

// Called when the game starts or when spawned
void AHYWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	AttributeSetRef = AttributeSystem;
}

// Called every frame
void AHYWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AHYWeaponBase::GenerateRandomDamageNumber(float _ComboFactor, float& _GeneratedDamage)
{
	// random rate is random float value between 0 and 1
	bool bIsCritical = FMath::FRandRange(0.0f, 100.0f) <= AttributeSystem->GetCurrentCriticalRate();

	// Make attack damage amount
	_GeneratedDamage = (bIsCritical)? AttributeSystem->GetCurrentCriticalDamage() : AttributeSystem->GetCurrentPhysicalDamage();
	_GeneratedDamage *= _ComboFactor; // And multiplies the combo factor to give player the advantage of maintaining a combo attack.

	return bIsCritical;
}

bool AHYWeaponBase::ResetAttributes()
{
	return AttributeSetRef->ResetAttributes();
}

bool AHYWeaponBase::Buff(FAttributeSetInfo _AttributeSetInfo)
{
	return AttributeSetRef->Buff(_AttributeSetInfo);
}

bool AHYWeaponBase::Debuff(FAttributeSetInfo _AttributeSetInfo)
{
	return AttributeSetRef->Debuff(_AttributeSetInfo);
}

