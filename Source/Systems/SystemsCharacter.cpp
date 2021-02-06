// Copyright Epic Games, Inc. All Rights Reserved.

#include "SystemsCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/AudioComponent.h"

//Inventory includes
#include "Inventory/Interactable.h"
#include "Inventory/AutoPickup.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/SystemsController.h"

//Weapons
#include "Weapons/Weapon.h"

//////////////////////////////////////////////////////////////////////////
// ASystemsCharacter

ASystemsCharacter::ASystemsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a follow camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FirstPersonCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FirstPersonCamera->bAutoActivate = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	bIsTalking = false;
	bIsInTalkRange = false;
	AssociatedPawn = nullptr;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//Inventory
	// Create the collection sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(200.f);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASystemsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASystemsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASystemsCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASystemsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASystemsCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASystemsCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASystemsCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASystemsCharacter::OnResetVR);


	InputComponent->BindAction("Talk", IE_Pressed, this, &ASystemsCharacter::ToggleTalking);

	InputComponent->BindAction("ChangeCamera", IE_Pressed, this, &ASystemsCharacter::ToggleCamera);

	InputComponent->BindAction("DropWeapon", IE_Pressed, this, &ASystemsCharacter::DetachWeapon);
}


void ASystemsCharacter::OnResetVR()
{
	// If Systems is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Systems.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASystemsCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ASystemsCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ASystemsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASystemsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASystemsCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !bIsTalking)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASystemsCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) && !bIsTalking)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void ASystemsCharacter::ToggleTalking()
{
	if (bIsInTalkRange)
	{
		//If we are in talk range handle the talk status and the UI
		bIsTalking = !bIsTalking;
		ToggleUI();
		if (bIsTalking && AssociatedPawn)
		{
			//The associated pawn is polite enough to face us when we talk to him!
			FVector Location = AssociatedPawn->GetActorLocation();
			FVector TargetLocation = GetActorLocation();

			AssociatedPawn->SetActorRotation((TargetLocation - Location).Rotation());
		}
	}
}

FDialog* ASystemsCharacter::RetrieveDialog(UDataTable* TableToSearch, FName RowName)
{
	if (!TableToSearch) return nullptr;

	//The table is valid - retrieve the given row if possible
	FString ContextString;
	return TableToSearch->FindRow<FDialog>(RowName, ContextString);
}

void ASystemsCharacter::GeneratePlayerLines(UDataTable& PlayerLines)
{
	//Get all the row names of the table
	TArray<FName> PlayerOptions = PlayerLines.GetRowNames();

	//For each row name try to retrieve the contents of the table
	for (auto It : PlayerOptions)
	{
		//Retrieve the contents of the table
		FDialog* Dialog = RetrieveDialog(&PlayerLines, It);

		if (Dialog)
		{
			//We retrieved a valid row - populate the questions array with our excerpts
			Questions.Add(Dialog->QuestionExcerpt);
		}
	}

	//Make sure to create a reference of the available line for later use
	AvailableLines = &PlayerLines;
}

void ASystemsCharacter::Talk(FString Excerpt, TArray<FSubtitle>& Subtitles)
{
	//Get all the row names based on our stored lines
	TArray<FName> PlayerOptions = AvailableLines->GetRowNames();

	for (auto It : PlayerOptions)
	{
		//Search inside the available lines table to find the pressed Excerpt from the UI
		FDialog* Dialog = RetrieveDialog(AvailableLines, It);

		if (Dialog && Dialog->QuestionExcerpt == Excerpt)
		{
			//We've found the pressed excerpt - assign our sfx to the audio comp and play it
			AudioComp->SetSound(Dialog->SFX);
			AudioComp->Play();

			//Update the corresponding subtitles
			Subtitles = Dialog->Subtitles;


			if (UI && AssociatedPawn && Dialog->bShouldAIAnswer)
			{
				//Calculate the total displayed time for our subtitles
				//When the subtitles end - the associated pawn will be able to talk to our character

				TArray<FSubtitle> SubtitlesToDisplay;

				float TotalSubsTime = 0.f;

				for (int32 i = 0; i < Subtitles.Num(); i++)
				{
					TotalSubsTime += Subtitles[i].AssociatedTime;
				}

				//Just a hardcoded value in order for the AI not to answer right after our subs.
				//It would be better if we expose that to our editor? Sure!
				TotalSubsTime += 1.f;

				//Tell the associated pawn to answer to our character after the specified time!
				AssociatedPawn->AnswerToCharacter(It, SubtitlesToDisplay, TotalSubsTime);

			}
			else if (!Dialog->bShouldAIAnswer) ToggleTalking();
			break;
		}
	}
}

void ASystemsCharacter::ToggleCamera()
{
	if (FirstPersonCamera == nullptr) return;
	if (FollowCamera == nullptr) return;

	if ((FirstPersonCamera) && (FirstPersonCamera->IsActive()))
	{
		//Activate 3rd person
		FirstPersonCamera->Deactivate();
		FollowCamera->Activate();
	}
	else
	{
		//Activate 1st person
		FollowCamera->Deactivate();
		FirstPersonCamera->Activate();
	}
}


//Inventory
void ASystemsCharacter::Tick(float Deltatime)
{
	Super::Tick(Deltatime);

	CollectAutoPickups();
	CheckForInteractables();
}

void ASystemsCharacter::CollectAutoPickups()
{
	// Get all overlapping Actors and store them in an array
	TArray<AActor*> CollectedActors;
	CollectionSphere->GetOverlappingActors(CollectedActors);

	ASystemsController* IController = Cast<ASystemsController>(GetController());

	// For each collected Actor
	for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
	{
		// Cast the actor to AAutoPickup
		AAutoPickup* const TestPickup = Cast<AAutoPickup>(CollectedActors[iCollected]);
		// If the cast is successful and the pickup is valid and active 
		if (TestPickup && !TestPickup->IsPendingKill())
		{
			TestPickup->Collect(IController);
		}
	}
}

void ASystemsCharacter::CheckForInteractables()
{
	// Create a LineTrace to check for a hit
	FHitResult HitResult;

	int32 Range = 500;
	FVector StartTrace = FollowCamera->GetComponentLocation();
	FVector EndTrace = (FollowCamera->GetForwardVector() * Range) + StartTrace;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	ASystemsController* IController = Cast<ASystemsController>(GetController());

	if (IController)
	{
		// Check if something is hit
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, QueryParams))
		{
			// Cast the actor to AInteractable
			AInteractable* Interactable = Cast<AInteractable>(HitResult.GetActor());
			// If the cast is successful
			if (Interactable)
			{
				IController->CurrentInteractable = Interactable;
				return;
			}
		}

		IController->CurrentInteractable = nullptr;
	}
}

void ASystemsCharacter::DetachWeapon()
{
	AttachedWeapon->DetachRootComponentFromParent();
	AttachedWeapon->WeaponMesh->SetSimulatePhysics(true);
	AttachedWeapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
