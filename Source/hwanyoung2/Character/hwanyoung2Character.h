// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
//#include "AIModule/Classes/GenericTeamAgentInterface.h"
#include "hwanyoung2.h"
#include "Components/SphereComponent.h"
#include "hwanyoung2Character.generated.h"


UCLASS(config = Game)
class Ahwanyoung2Character : public ACharacter/*, public UGenericTeamAgentInterface*/
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Collection sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

#pragma region Character Gameplay Input Action
	/** MappingContext */
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* InputMapping;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Walk Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WalkAction;

#pragma endregion

private:
	//Helper function for 
	void Initialize();

public:

	Ahwanyoung2Character();

	//Updates the AbilitysystemComponent's actorInfo, especially in a multiplayer environment
	//Gets called on the server (so basically my end)
	virtual void PossessedBy(AController* NewController) override;


	UFUNCTION(BlueprintCallable, Category = "HY_Character|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "HY_Character|Camera")
	FVector GetStartingCameraBoomLocation();

	UFUNCTION(BlueprintCallable, Category = "HY_Character|Climbing")
	virtual void ClimbingLineTrace(bool& _BodyHit, bool& _HeadHit, FHitResult& _BodyHitResult, FHitResult& _HeadHitResult);

	UFUNCTION(BlueprintCallable, Category = "HY_Character|Climbing")
	virtual bool ClimbDownLineTrace();

	UFUNCTION(BlueprintCallable, Category = "HY_Character|Climbing")
	virtual void StopClimbing();

	UFUNCTION(BlueprintCallable, Category = "HY_Character|Climbing")
	virtual void ClimbUpLedge();

	UFUNCTION(BlueprintCallable, Category = "HY_Character|Climbing")
	virtual bool CheckHasReachedFloor();

	UFUNCTION(BlueprintCallable, Category = "HY_Character|Climbing")
	virtual void ToggleClimbing();

	UFUNCTION(BlueprintNativeEvent, Category = "HY_Character|Climbing")
	void ClimbDownFromLedge();

	virtual void ClimbDownFromLedge_Implementation();

	// For animation on ended event delegate
	UFUNCTION(BlueprintCallable, Category = "HY_Character|Climbing")
	virtual void StartClimbingDown(UAnimMontage* _Montage, bool _Interrupted);

	UFUNCTION(BlueprintCallable)
	virtual void UseStamina();

	UFUNCTION(BlueprintCallable)
	virtual void ReplenishStamina();

	/** Returns CameraBoom subobject **/
	class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HY_Character|Stamina")
	FTimerHandle StaminaReduceTimer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HY_Character|Stamina")
	FTimerHandle StaminaRechargeTimer;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HY_Character|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HY_Character|Camera")
	FVector StartingCameraBoomLocation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HY_Character|Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HY_Character|Camera")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HY_Character|Climbing")
	FVector ClimbableSurfaceNormal;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hwanyoung|Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hwanyoung|Climbing")
	bool bIsClimbing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Hwanyoung|Climbing")
	bool bIsClimbingTriggerOn;

	/** Called for movement input */
	UFUNCTION(BlueprintCallable)
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Jump() override;

	/** Called for checking for the closest Interactable in sight and in range*/
	void CheckForInteractables();

	/** Called for collecting automatic pick-upable interactables*/
	void CollectAutoPickups();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	// Climbing down animation montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAnimMontage> ClimbingDownLedgeMontage;

	// Climbing ledge up animation montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAnimMontage> ClimbingLedgeUpMontage;

	/*virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);
	virtual FGenericTeamId GetGenericTeamId() const;*/

};

