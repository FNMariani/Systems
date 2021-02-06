// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Subtitle.h"
#include "AICharacter.generated.h"

UCLASS()
class SYSTEMS_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex);

	UFUNCTION()
	void Talk(USoundBase* SFX, TArray<FSubtitle> Subs);

	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex);

public:
	/*Answers to the character after a specified delay*/
	void AnswerToCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay);

	/*Retrieves the corresponding character lines*/
	UDataTable* GetPlayerLines() { return PlayerLines; }

protected:
	/*If the player is inside this box component he will be able to initiate a conversation with the pawn*/
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxComp;

	/*The audio component responsible for playing the dialog coming from this pawn*/
	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* AudioComp;

	/*The player lines - each pawn can offer different dialog options for our character*/
	UPROPERTY(EditAnywhere, Category = DialogSystem)
	UDataTable* PlayerLines;

	/*The ai lines*/
	UPROPERTY(EditAnywhere, Category = DialogSystem)
	UDataTable* AILines;


	//Random phrase when player is in range
	UPROPERTY(EditAnywhere, Category = "Phrases")
	UDataTable* AIRandomLines;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, Category = "Phrases");
	class UTextRenderComponent* TextRenderActor;
};
