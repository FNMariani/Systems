// by FNM.

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/Interactable.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class SYSTEMS_API AWeapon : public AInteractable
{
	GENERATED_BODY()
	
public:
	AWeapon();
	
	void Interact_Implementation(APlayerController* Controller) override;

public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
	FName WeaponSocket;
};
