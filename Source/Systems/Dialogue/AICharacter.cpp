// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "../SystemsCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/TextRenderComponent.h" 

// Sets default values
AAICharacter::AAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init the box and audio comps
	BoxComp = CreateDefaultSubobject<UBoxComponent>(FName("BoxComp"));
	BoxComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	AudioComp = CreateDefaultSubobject<UAudioComponent>(FName("AudioComp"));
	AudioComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


	SphereComp = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	SphereComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TextRenderActor = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PlayerName"));
	TextRenderActor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Register the begin and end overlap functions
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnBoxOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AAICharacter::OnBoxEndOverlap);


	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAICharacter::OnSphereOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &AAICharacter::OnSphereEndOverlap);
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAICharacter::OnBoxOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ASystemsCharacter>())
	{
		ASystemsCharacter* Char = Cast<ASystemsCharacter>(OtherActor);
		Char->SetTalkRangeStatus(true);
		Char->GeneratePlayerLines(*PlayerLines);
		Char->SetAssociatedPawn(this);
	}
}

void AAICharacter::OnBoxEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
{
	if (OtherActor->IsA<ASystemsCharacter>())
	{
		ASystemsCharacter* Char = Cast<ASystemsCharacter>(OtherActor);
		Char->SetTalkRangeStatus(false);
		Char->SetAssociatedPawn(nullptr);
	}
}

void AAICharacter::Talk(USoundBase* SFX, TArray<FSubtitle> Subs)
{
	ASystemsCharacter* Char = Cast<ASystemsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//Play the corresponding sfx
	AudioComp->SetSound(SFX);
	AudioComp->Play();

	//Tell the UI to update with the new subtitles
	Char->GetUI()->UpdateSubtitles(Subs);
}

void AAICharacter::OnSphereOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AIRandomLines) return;

	ASystemsCharacter* MainChar = Cast<ASystemsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (OtherActor == MainChar)
	{
		if (OtherComp->ComponentHasTag("Player"))
		{		
			//Get all the row names of the table
			TArray<FName> PlayerOptions = AIRandomLines->GetRowNames();

			//Select random phrase
			int OptionNumber = FMath::RandRange(0, PlayerOptions.Num() - 1);
			FString ContextString;
			FSubtitle* Subtitle = AIRandomLines->FindRow<FSubtitle>(PlayerOptions[OptionNumber], ContextString);

			TextRenderActor->SetText(*Subtitle->Subtitle);
		}
	}
}

void AAICharacter::OnSphereEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherIndex)
{
	TextRenderActor->SetText("");
}

void AAICharacter::AnswerToCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay)
{
	if (!AILines) return;

	//Retrieve the corresponding line
	FString ContextString;
	FDialog* Dialog = AILines->FindRow<FDialog>(PlayerLine, ContextString);

	ASystemsCharacter* MainChar = Cast<ASystemsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Dialog && MainChar)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName("Talk"), Dialog->SFX, Dialog->Subtitles);

		//Talk to the player after the delay time has passed
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, delay, false);
	}
}