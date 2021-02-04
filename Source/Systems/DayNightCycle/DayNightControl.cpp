// by FNM.


#include "DayNightControl.h"

// Sets default values
ADayNightControl::ADayNightControl()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ADayNightControl::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADayNightControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LightSource)
	{
		LightSource->AddActorLocalRotation(FRotator((DeltaTime * TurnRate), 0, 0));
	}

	if (Sun)
	{
		FOutputDeviceNull Ar;
		Sun->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), Ar, NULL, true);
	}
}

