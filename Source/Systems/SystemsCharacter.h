// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "GameFramework/Character.h"
#include "Dialogue/AICharacter.h"
#include "Dialogue/DialogUI.h"
#include "SystemsCharacter.generated.h"

UCLASS(config=Game)
class ASystemsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;


	//Inventory
	/** Collection sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

public:
	ASystemsCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns FirstPersonCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

private:
	/*True if the player is currently talking with any pawn*/
	bool bIsTalking;

	/*True if the player is inside a valid range to start talking to a pawn*/
	bool bIsInTalkRange;

	/*Initiates or terminates a conversation*/
	void ToggleTalking();

	/*The pawn that the player is currently talking to*/
	AAICharacter* AssociatedPawn;

	/*A reference to our lines - retrieved from the associated pawn*/
	UDataTable* AvailableLines;

	/*Searches in the given row inside the specified table*/
	FDialog* RetrieveDialog(UDataTable* TableToSearch, FName RowName);

public:
	/*Generates the player lines*/
	void GeneratePlayerLines(UDataTable& PlayerLines);

	/*This array is essentially an Array of Excerpts from our dialogs!*/
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Questions;

	/*Performs the actual talking - informs the associated pawn if necessary in order to answer
	The subtitles array contain all the subtitles for this talk - it should be passed to our UI*/
	UFUNCTION(BlueprintCallable, Category = DialogSystem)
	void Talk(FString Excerpt, TArray<FSubtitle>& Subtitles);

	/*Enables / disables our talk ability. The player can't talk if he's not in a valid range*/
	void SetTalkRangeStatus(bool Status) { bIsInTalkRange = Status; }

	/*Sets a new associated pawn*/
	void SetAssociatedPawn(AAICharacter* Pawn) { AssociatedPawn = Pawn; }

	/*Retrieves the UI reference*/
	UDialogUI* GetUI() { return UI; }

	void ToggleCamera();

protected:
	/*The component responsible for playing our SFX*/
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComp;

	/*Opens or closes the conversation UI*/
	UFUNCTION(BlueprintImplementableEvent, Category = DialogSystem)
	void ToggleUI();

	/*UI Reference*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UDialogUI* UI;


//Inventory
public:
	virtual void Tick(float DeltaTime) override;

protected:
	/** Function to collect every AutoPickup in range. */
	void CollectAutoPickups();

	/** Function to check for the closest Interactable in sight and in range. */
	void CheckForInteractables();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString TextToPickup;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class AWeapon* AttachedWeapon;

	UFUNCTION()
	void DetachWeapon();
};

