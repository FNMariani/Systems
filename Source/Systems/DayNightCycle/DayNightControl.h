// by FNM.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DirectionalLight.h" 
#include "Misc/OutputDeviceNull.h"
#include "GameFramework/Actor.h"
#include "DayNightControl.generated.h"

UCLASS()
class SYSTEMS_API ADayNightControl : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADayNightControl();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Sky")
	AActor* Sun;

	UPROPERTY(EditAnywhere, Category = "Sky")
	ADirectionalLight* LightSource;

	UPROPERTY(EditAnywhere, Category = "Sky")
	float TurnRate;
};
