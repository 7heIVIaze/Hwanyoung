// Copyright Epic Games, Inc. All Rights Reserved.

#include "hwanyoung2Character.h"
#include "Player/HYPlayerCharacController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interactables/HYInteractableActor.h"
#include "Inventory/InventoryItem.h"
#include "Interactables/Items/HYAutomaticPickUp.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/HYWorldSettings.h"
#include "HYCharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"

#include "Teammates/HYInteractionWidget.h"


//////////////////////////////////////////////////////////////////////////
// Ahwanyoung2Character
Ahwanyoung2Character::Ahwanyoung2Character()
{
	Initialize();
}

Ahwanyoung2Character::Ahwanyoung2Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHYCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	Initialize();
	MovementComponent = Cast<UHYCharacterMovementComponent>(GetCharacterMovement());
}

void Ahwanyoung2Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void Ahwanyoung2Character::ClimbDownFromLedge_Implementation()
{
	MovementComponent->TryClimbingFromTop();
}

void Ahwanyoung2Character::TryClimbing()
{
	MovementComponent->TryClimbing();

	UE_LOG(LogTemp, Log, TEXT("Is Climbing: %s"), MovementComponent->IsClimbing() ? TEXT("True") : TEXT("False"));
}

void Ahwanyoung2Character::StopClimbing()
{
	MovementComponent->CancelClimbing();
}

void Ahwanyoung2Character::UseStamina()
{
	
}

void Ahwanyoung2Character::ReplenishStamina()
{
}

void Ahwanyoung2Character::ToggleStaminaBar(bool bIsVisibility)
{
}

void Ahwanyoung2Character::UpdateInteractionWidget()
{
	// Validation check
	if (InteractionWidget == nullptr)
	{
		InteractionWidget = CreateWidget<UUserWidget>(Controller, InteractionWidgetClass);
		InteractionWidget->AddToViewport();
	}

	InteractionWidget->SetText();
}

float Ahwanyoung2Character::GetStartingCameraBoomArmLength()
{
	return 0.0f;
}

FVector Ahwanyoung2Character::GetStartingCameraBoomLocation()
{
	return FVector();
}

#pragma region Deprecated(Replaced to HYCharacterMovementComp)
//void Ahwanyoung2Character::StartClimbingDown(UAnimMontage* _Montage, bool _Interrupted)
//{
//	if (_Montage == ClimbingDownLedgeMontage)
//	{
//		bool BodyHit, HeadHit;
//		FHitResult BodyHitResult, HeadHitResult;
//		ClimbingLineTrace(BodyHit, HeadHit, BodyHitResult, HeadHitResult);
//
//		FVector TargetRelativeLocation = BodyHitResult.Location + 
//			(GetActorForwardVector() * -1.0f * GetCapsuleComponent()->GetUnscaledCapsuleRadius());
//
//		FRotator TargetRelativeRotation = FRotator(0.0f, UKismetMathLibrary::MakeRotFromX(BodyHitResult.Normal * -1.0f).Yaw, 0.0f);
//
//		UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), TargetRelativeLocation,
//			TargetRelativeRotation, false, false, 0.2f, false, EMoveComponentAction::Move, FLatentActionInfo());
//
//		bIsClimbing = true;
//	}
//
//	else if (_Montage == ClimbingLedgeUpMontage)
//	{
//		ClimbUpLedge();
//	}
//}

//void Ahwanyoung2Character::ClimbingLineTrace(bool& _BodyHit, bool& _HeadHit, FHitResult& _BodyHitResult, FHitResult& _HeadHitResult)
//{
//	FVector FirstStartLocation = GetActorLocation();
//	FVector FirstEndLocation = FirstStartLocation + GetActorForwardVector() * 100.0f;
//	FVector SecondStartLocation = FirstStartLocation + GetActorUpVector() * 80.0f;
//	FVector SecondEndLocation = SecondStartLocation + GetActorForwardVector() * 100.0f;
//
//	// Do line trace from body
//	_BodyHit = GetWorld()->LineTraceSingleByChannel(_BodyHitResult, FirstStartLocation, FirstEndLocation, ECC_Visibility);
//
//	// if body hit is true
//	if (_BodyHit)
//	{
//		// do line trace from head point
//		_HeadHit = GetWorld()->LineTraceSingleByChannel(_HeadHitResult, SecondStartLocation, SecondEndLocation, ECC_Visibility);
//
//		ClimbableSurfaceNormal = FVector::ZeroVector;
//
//		ClimbableSurfaceNormal += _BodyHitResult.ImpactNormal;
//
//		// Make Normal
//		ClimbableSurfaceNormal = ClimbableSurfaceNormal.GetSafeNormal();
//	}
//}
//
//bool Ahwanyoung2Character::ClimbDownLineTrace()
//{
//	if (GetCharacterMovement()->IsFalling()) return false;
//
//	const FVector ActorLocation = GetActorLocation();
//	const FVector ForwardVector = GetActorForwardVector();
//	const FVector DownVector = -GetActorUpVector();
//
//	const FVector WalkableSufaceTraceStart = ActorLocation + ForwardVector * 50.0f;
//	const FVector WalkableSufaceTraceEnd = WalkableSufaceTraceStart + DownVector * 150.0f;
//	FHitResult WalkableSurfaceTraceHit;
//
//	// Do first line trace
//	bool bWalkableHit = GetWorld()->LineTraceSingleByChannel(WalkableSurfaceTraceHit, 
//		WalkableSufaceTraceStart, WalkableSufaceTraceEnd, ECC_Visibility);
//
//	FColor LineColor = bWalkableHit ? FColor::Green : FColor::Red;
//	DrawDebugLine(GetWorld(), WalkableSufaceTraceStart, WalkableSufaceTraceEnd, LineColor, false, 1.0f, 0, 2.0f);
//
//	// Do second line trace
//	const FVector LedgeTraceStart = WalkableSurfaceTraceHit.TraceStart + ForwardVector * 50.0f;
//	const FVector LedgeTraceEnd = LedgeTraceStart + DownVector * 300.0f;
//	FHitResult LedgeTraceHit;
//
//	bool bLedgeHit = GetWorld()->LineTraceSingleByChannel(LedgeTraceHit, LedgeTraceStart, LedgeTraceEnd, ECC_Visibility);
//
//	LineColor = bLedgeHit ? FColor::Green : FColor::Red;
//	DrawDebugLine(GetWorld(), LedgeTraceStart, LedgeTraceEnd, LineColor, false, 1.0f, 0, 2.0f);
//
//	// If the first hit is true and second hit is false, then player can start to climb ledge.
//	if (WalkableSurfaceTraceHit.bBlockingHit && !LedgeTraceHit.bBlockingHit)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//bool Ahwanyoung2Character::CheckHasReachedFloor()
//{
//	const FVector DownVector = -GetActorUpVector();
//	const FVector StartOffset = DownVector * 50.0f;
//
//	const FVector Start = GetActorLocation() + StartOffset;
//	const FVector End = Start + DownVector * 75.0f;
//
//	FHitResult PossibleFloorHit;
//
//	bool bClimbableHit = GetWorld()->LineTraceSingleByChannel(PossibleFloorHit, Start, End, ECC_Visibility);
//
//	/*
//	* For Debugbing
//	//FColor LineColor = bClimbableHit ? FColor::Green : FColor::Red;
//	//DrawDebugLine(GetWorld(), Start, End, LineColor, false, 1.0f, 0, 2.0f);
//
//	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%f"), UKismetMathLibrary::Quat_UnrotateVector(GetActorQuat(), GetCharacterMovement()->Velocity).Z));
//	*/
//
//	if (!bClimbableHit)
//	{
//		return false;
//	}
//
//	// Check only if character is close to floor and move to down.
//	const bool bFloorReached =
//		bClimbableHit &&
//		UKismetMathLibrary::Quat_UnrotateVector(GetActorQuat(), GetCharacterMovement()->Velocity).Z < -10.0f;
//
//	//* For Debugbing
//	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%s"), FVector::Parallel(-PossibleFloorHit.ImpactNormal, FVector::UpVector) ? TEXT("true") : TEXT("false")));
//
//	if (bFloorReached)
//	{
//		return true;
//	}
//
//	return false;
//}
//
//void Ahwanyoung2Character::ToggleClimbing()
//{
//	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
//	GetCharacterMovement()->bOrientRotationToMovement = false;
//	GetCharacterMovement()->StopMovementImmediately();
//
//	if (GetWorldTimerManager().IsTimerActive(StaminaRechargeTimer))
//	{
//		GetWorld()->GetTimerManager().ClearTimer(StaminaRechargeTimer);
//	}
//	GetWorld()->GetTimerManager().SetTimer(StaminaReduceTimer, this, &Ahwanyoung2Character::UseStamina, 0.05f, true);
//}
//
//
//void Ahwanyoung2Character::ClimbUpLedge()
//{
//	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
//	GetCharacterMovement()->bOrientRotationToMovement = true;
//	FVector FowardVector = GetActorForwardVector() * 500.0f;
//
//	if (GetWorldTimerManager().IsTimerActive(StaminaReduceTimer))
//	{
//		GetWorld()->GetTimerManager().ClearTimer(StaminaReduceTimer);
//	}
//	GetWorld()->GetTimerManager().SetTimer(StaminaRechargeTimer, this, &Ahwanyoung2Character::ReplenishStamina, 0.05f, true);
//}
#pragma endregion

void Ahwanyoung2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ELevelID"), true);
	const AHYWorldSettings* WS = Cast<AHYWorldSettings>(GetWorld()->GetWorldSettings());
	UE_LOG(LogTemp, Log, TEXT("Level Index: %s"), *EnumPtr->GetNameStringByValue((int64)WS->LevelIndex));
	LevelID = WS->LevelIndex;
	UE_LOG(LogTemp, Log, TEXT("Level Index: %s"), *EnumPtr->GetNameStringByValue((int64)LevelID));
}

//void Ahwanyoung2Character::SetGenericTeamId(const FGenericTeamId& NewTeamID)
//{
//}
//
//FGenericTeamId Ahwanyoung2Character::GetGenericTeamId() const
//{
//	return FGenericTeamId();
//}

void Ahwanyoung2Character::CheckForInteractables()
{
	// Creates a LineTrace (similar to RayCast) to check for a hit
	FHitResult HitResult;

	// The range of area the system should check for interactables
	int32 Range = 800; //this can be changed

	//The start of the trace is the transform of the follow camera
	FVector StartTrace = FollowCamera->GetComponentLocation();

	//And the end of the trace is the 500 units ahead of the start trace
	FVector EndTrace = (FollowCamera->GetForwardVector() * Range) + StartTrace;

	//Keeps track of parameters passed into collision function, assuming that 
	//there are multiple collided actors that is passed through the collision function
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); //we are ignoring this, which is the character

	//Similarly to how GetComponent<>() function works, we get the controller
	//object attached to the character and cast it as player character controller class
	AHYPlayerCharacController* IController = Cast<AHYPlayerCharacController>(GetController());


	//Checking if something is hit by the line cast within the range
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace,
		ECC_Visibility, QueryParams)) {

		//Cast the actor to AInteractable
		if (AHYInteractableActor* Interactable = Cast<AHYInteractableActor>(HitResult.GetActor())) {
			IController->CurrentInteractable = Interactable;
			UpdateInteractionWidget();
			return;
		}
		else
		{
			IController->CurrentInteractable = nullptr;
			UpdateInteractionWidget();
		}
	}
	else {
		IController->CurrentInteractable = nullptr;
		UpdateInteractionWidget();
	}


}

void Ahwanyoung2Character::CollectAutoPickups()
{
	// Stores all the overlapping actors in an array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);

	AHYPlayerCharacController* IController = Cast<AHYPlayerCharacController>(GetController());

	for (int32 indCollect = 0; indCollect < CollectedActors.Num(); ++indCollect) {
		AHYAutomaticPickUp* const Pickup = Cast<AHYAutomaticPickUp>(CollectedActors[indCollect]);
		if (Pickup && !Pickup->IsPendingKill()) {
			Pickup->Collect(IController);
		}
	}
}


void Ahwanyoung2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForInteractables();
	CollectAutoPickups();
}

//////////////////////////////////////////////////////////////////////////
// Input

void Ahwanyoung2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Ahwanyoung2Character::Move);

		//Walking
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Triggered, this, &Ahwanyoung2Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Ahwanyoung2Character::Look);

	}

}

void Ahwanyoung2Character::ChangeLevelIndex(FString LevelName)
{
	if (LevelName == TEXT("Sprink_TEST"))
	{
		LevelID = ELevelID::Spring;
	}
	else
	{
		LevelID = ELevelID::Loading;
	}
}


void Ahwanyoung2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 

		if (MovementComponent->IsClimbing())
		{
			const FVector UpDirection = FVector::CrossProduct(
				MovementComponent->GetClimbSurfaceNormal(),
				-GetActorRightVector());

			const FVector ClimbRightDirection = FVector::CrossProduct(
				MovementComponent->GetClimbSurfaceNormal(),
				GetActorUpVector());

			AddMovementInput(ClimbRightDirection, MovementVector.X);
			AddMovementInput(UpDirection, MovementVector.Y);

		}
		else
		{
			AddMovementInput(RightDirection, MovementVector.X);
			AddMovementInput(ForwardDirection, MovementVector.Y);
		}
	}
}

void Ahwanyoung2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void Ahwanyoung2Character::Jump()
{
	// if the character is climbing, stop
	if (MovementComponent->IsClimbing())
	{
		StopClimbing();
	}
	// else then check the character could start to climb
	else
	{
		if (bIsClimbingTriggerOn)
		{
			ClimbDownFromLedge();
		}
		else
		{
			Super::Jump();
			TryClimbing();
		}
	}
}

void Ahwanyoung2Character::Initialize()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetOffset = FVector(-20.0, 0.0f, 110.0f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(200.f);
}





//void Ahwanyoung2Character::RemoveCharacterAbilities()
//{
//	//if the object doesn't have the authority, ability system component is not valid
//	//or character abilities are not given in the ability system component,
//	//we don't do anything and just return
//	if (GetLocalRole() != ROLE_Authority ||
//		!AbilitySystemComponent.IsValid() ||
//		!AbilitySystemComponent->CharacterAbilitiesGiven)
//	{
//		return;
//	}
//
//	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
//	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
//	{
//		if (Spec.SourceObject == this && CharacterAbilities.Contains(Spec.Ability->GetClass()))
//		{
//			AbilitiesToRemove.Add(Spec.Handle); //spec.handle is an instance of the ability
//		}
//	}
//
//	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
//	{
//		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
//	}
//
//	AbilitySystemComponent->CharacterAbilitiesGiven = false;
//}

//void Ahwanyoung2Character::Die()
//{
//	RemoveCharacterAbilities();
//
//	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	GetCharacterMovement()->GravityScale = 0;
//	GetCharacterMovement()->Velocity = FVector(0); //disabling character movement when death
//
//	OnCharacterDied.Broadcast(this);
//
//	if (AbilitySystemComponent.IsValid())
//	{
//		AbilitySystemComponent->CancelAbilities();
//
//		FGameplayTagContainer EffectsTagsToRemove;
//		EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
//		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);
//		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
//	}
//
//	//playing death anim:
//	if (DeathMontage)
//	{
//		PlayAnimMontage(DeathMontage);
//	}
//	else
//	{
//		FinishDying();
//	}
//}
