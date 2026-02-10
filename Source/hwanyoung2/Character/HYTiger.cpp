// Copyright 2024. TEAM DAON. All rights reserved.

#include "HYTiger.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HYPlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NavigationSystem.h"
#include "Weapons/Projectiles/HYProjectileBase.h"

AHYTiger::AHYTiger()
{
	
}

void AHYTiger::BeginPlay()
{
	Super::BeginPlay();

	if (LungeCurveVector)
	{
		FOnTimelineVector LungeTimelineVector;
		LungeTimelineVector.BindUFunction(this, TEXT("LungeTimelineUpdate"));
		LungeTimeline.AddInterpVector(LungeCurveVector, LungeTimelineVector);

		FOnTimelineEvent LungeTimelineEnded;
		LungeTimelineEnded.BindUFunction(this, FName("LungeTimelineFinished"));
		LungeTimeline.SetTimelineFinishedFunc(LungeTimelineEnded);
	}

	if (JumpCurveFloat)
	{
		FOnTimelineFloat JumpFloatCallback;
		JumpFloatCallback.BindUFunction(this, TEXT("JumpingTimelineUpdate"));
		JumpTimeline.AddInterpFloat(JumpCurveFloat, JumpFloatCallback);

		FOnTimelineEvent JumpTimelineEnded;
		JumpTimelineEnded.BindUFunction(this, FName(TEXT("JumpingTimelineFinished")));
		JumpTimeline.SetTimelineFinishedFunc(JumpTimelineEnded);
	}
}

void AHYTiger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LungeTimeline.IsPlaying())
	{
		LungeTimeline.TickTimeline(DeltaTime);
	}
}

void AHYTiger::Lunge(AActor* _AttackTarget)
{
	PlayAnimMontage(TigerLungeMontage);
	
	AttackAnimStart();

	Target = _AttackTarget;
	// Rotate Character to see player
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(), _AttackTarget->GetActorLocation()));

	// Change Collision response to overlap player character
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	LungeTimeline.PlayFromStart();

}

void AHYTiger::OnMontageEnded(UAnimMontage* _Montage, bool _Interrupted)
{
	// If died montage
	if (_Montage == DeathAnimMontage)
	{
		Super::DiedMontageEnded();
	}
	// if the last montage is lunge, set attack anim ended.
	else if (_Montage == TigerLungeMontage)
	{
		Target = nullptr;
		AttackAnimEnd();
	}
	// if the last montage is roaring, call use ult skill again
	else if (_Montage == TigerRoarMontage)
	{
		UseUltSkill(Target);
	}
	// if the last montage is bite dash, set attack anim ended.
	else if (_Montage == TigerBitingMontage)
	{
		Target = nullptr;
		AttackAnimEnd();
	}
}

void AHYTiger::LungeTimelineUpdate(FVector Location)
{
	FVector ActorLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	// Lunge Motion
	SetActorLocation(FVector(
		FMath::Lerp(ActorLocation.X, TargetLocation.X, Location.X),
		FMath::Lerp(ActorLocation.Y, TargetLocation.Y, Location.Y),
		FMath::Lerp(CurrentLocation.Z, TargetLocation.Z + 3000.0f, Location.X)
	));
}

void AHYTiger::LungeTimelineFinished()
{
	LandedDelegate.AddDynamic(this, &AHYTiger::OnLand);
}

void AHYTiger::OnLand(const FHitResult& Hit)
{
	LandedDelegate.RemoveDynamic(this, &AHYTiger::OnLand);
	GetCharacterMovement()->StopMovementImmediately();

	GetWorld()->SpawnActor<AActor>(LungeAOEClass, GetActorLocation(), FRotator(0.0f), FActorSpawnParameters());

	// Restore Collision response to block player character
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	OnATKActionCompleted.Broadcast(Target);
}

void AHYTiger::CrackoftheEarth(AActor* _AttackTarget)
{
	Target = _AttackTarget;
	CurrentLocation = GetActorLocation();
	
	// Rotate Character to see player
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(), _AttackTarget->GetActorLocation()));

	JumpTimeline.PlayFromStart();
}

void AHYTiger::JumpingTimelineUpdate(float LocationZ)
{
	// update Z of location, using timeline to seem like jumping
	SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y,
		FMath::Lerp(CurrentLocation.Z, CurrentLocation.Z + 2400.0f, LocationZ)));
}

void AHYTiger::JumpingTimelineFinished()
{
	MakeTrailProjectile();
}

void AHYTiger::MakeTrailProjectile()
{
	AHYProjectileBase* EarthTrail = GetWorld()->SpawnActor<AHYProjectileBase>(EarthTrailClass,
		GetMesh()->GetComponentLocation() + GetActorForwardVector() * 100.0f,
		GetActorRotation(), FActorSpawnParameters());

	EarthTrail->OnProjectileImpact.AddDynamic(this, &AHYTiger::TrailDamage);

	// This skill is 3 times combo.
	if (CrackUseCount < 2)
	{
		CrackUseCount++;
		FTimerHandle Timer;
		GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([&]()
		{
			CrackoftheEarth(Target);
		}), 1.0f, false);
	}
	else
	{
		CrackUseCount = 0;
		OnATKActionCompleted.Broadcast(Target);
	}
}

void AHYTiger::TrailDamage(AActor* OtherActor, const FHitResult& Hit)
{
	if (auto PlayerCharacter = Cast<AHYPlayerCharacterBase>(OtherActor))
	{
		PlayerCharacter->TakeDamage(
			FDamageInfo(15.0f, EDamageType::Projectile, EDamageReactionType::ProjectileHitReaction,
				FCrowdControlInfo(), false, true, false, true),
			this, 0.0f, 0.0f, Hit);
	}
}

void AHYTiger::UseUltSkill(AActor* _AttackTarget)
{
	Target = nullptr;

	if (bHasActivateUlt)
	{
		UltJump(_AttackTarget);
	}
	else
	{
		Roar(_AttackTarget);
	}
}

void AHYTiger::Roar(AActor* _AttackTarget)
{
	bHasActivateUlt = true;

	PlayAnimMontage(TigerRoarMontage);
	Target = _AttackTarget;
}

void AHYTiger::OnRoarNotify()
{
	SlashBleedProbability += 50.0f;
	DamageFactor *= 2.0f;
}

void AHYTiger::UltJump(AActor* _AttackTarget)
{
	PlayAnimMontage(TigerChargeJumpMontage);

	// Make jump
	FVector LaunchVelocity, StartLocation, TargetLocation;
	StartLocation = GetActorLocation();
	TargetLocation = _AttackTarget->GetActorLocation();
	TargetLocation.Z += 200.0f;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, StartLocation, TargetLocation, 0.0f, 0.75f);

	LaunchCharacter(LaunchVelocity, true, true);

	// to make overlap player character
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Target = _AttackTarget;

	LandedDelegate.AddDynamic(this, &AHYTiger::OnUltLand);
}

void AHYTiger::OnUltLand(const FHitResult& Hit)
{
	LandedDelegate.RemoveDynamic(this, &AHYTiger::OnUltLand);

	GetCharacterMovement()->StopMovementImmediately();
	GetWorld()->SpawnActor<AActor>(UltJumpAOEClass, GetActorLocation(), FRotator(0.0f), FActorSpawnParameters());

	// to restore the response
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([&]() {
		SpinningSlash(Target);
	}), 3.0f, false);
}

void AHYTiger::SpinningSlash(AActor* _AttackTarget)
{
	PlayAnimMontage(TigerSpinningSlashMontage);
}

void AHYTiger::OnSpinningNotified()
{
	GetWorld()->SpawnActor<AActor>(SpinningSlashAOEClass, GetActorLocation(), FRotator(0.0f), FActorSpawnParameters());

	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([&]() {
		Bite(Target);
	}), 2.0f, false);
}

void AHYTiger::Bite(AActor* _AttackTarget)
{
	// to make overlap player character
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	PlayAnimMontage(TigerBitingMontage);

	UNiagaraComponent* TrailComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		NS_Trail, GetMesh(), FName("None"),
		FVector(0.0f), FRotator(0.0f), EAttachLocation::KeepRelativeOffset, true);

	FVector StartLocation = GetActorLocation(), TargetLocation = _AttackTarget->GetActorLocation();

	FVector DashDirectionSpeed = (TargetLocation - StartLocation).GetSafeNormal(0.0001f) * 2000.0f;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);

	/*FNavLocation StartPathLocation;
	FNavLocation EndPathLocation;
	NavSystem->ProjectPointToNavigation(GetActorLocation(), StartPathLocation, INVALID_NAVEXTENT);
	NavSystem->ProjectPointToNavigation(DashDirectionSpeed, EndPathLocation, INVALID_NAVEXTENT);*/


	// Make dash
	LaunchCharacter(DashDirectionSpeed, true, true);
	//LaunchCharacter(EndPathLocation.Location, true, true);

	UNiagaraComponent* TrailComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this, NS_DashTrail, GetActorLocation(), GetActorRotation());
}

void AHYTiger::OnInitBiteNotify()
{
	TigerBite(true);
}

void AHYTiger::OnFinalBiteNotify()
{
	TigerBite(false);
}

void AHYTiger::TigerBite(bool _IsFirstBite)
{
	FVector StartLocation = GetMesh()->GetComponentLocation();
	FVector EndLocation = StartLocation + FVector(0.0f, 0.0f, 350.0f);
	float Radius = _IsFirstBite ? 350.0f : 200.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn); 
	TArray<AActor*> IgnoreActors;
	FHitResult HitResult;

	bool HasTraced = UKismetSystemLibrary::SphereTraceSingleForObjects(
		this, StartLocation, EndLocation, Radius, ObjectTypes,
		false, IgnoreActors, EDrawDebugTrace::None, HitResult, true
	);

	if (HasTraced)
	{
		if (IDamagable* DamagableInterface = Cast<IDamagable>(HitResult.GetActor()))
		{
			float DamageAmount = _IsFirstBite ? 35.0f : 50.0f;
			FCrowdControlInfo CrowdControl;

			FDamageInfo DamageInfo(
				DamageAmount, EDamageType::CloseCombat, EDamageReactionType::CloseCombatHitReaction,
				CrowdControl, true, false, false, true);

			DamagableInterface->TakeDamage(DamageInfo, this, 0.0f, 0.0f, HitResult);
		}

		if (_IsFirstBite)
		{
			// dash again
			FVector LaunchVelocity =
				HitResult.GetActor()->GetActorLocation() - GetActorLocation();
			LaunchVelocity = LaunchVelocity.GetSafeNormal(0.0001) * -7500.0f;


			LaunchCharacter(LaunchVelocity, true, false);
		}
		else
		{
			// Rising UpperCut.
			LaunchCharacter(FVector(0.0f, 0.0f, 1800.0f), false, true);
		}
	}

	// to restore the response
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}