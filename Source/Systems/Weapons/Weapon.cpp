// by FNM.


#include "Weapon.h"
#include "../Inventory/SystemsController.h"

#include "../SystemsCharacter.h"
#include "Kismet/GameplayStatics.h" 

AWeapon::AWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	RootComponent = Cast<USceneComponent>(WeaponMesh);

	Super::Name = "Weapon";
	Super::Action = "pickup";

	WeaponSocket = TEXT("hand_lSocket");
}

void AWeapon::Interact_Implementation(APlayerController* Controller)
{
	Super::Interact_Implementation(Controller);

	ASystemsCharacter* MainChar = Cast<ASystemsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (MainChar)
	{
		//If has a weapon in hand drop it
		if(MainChar->GetAttachedWeapon())
		{
			MainChar->DetachWeapon();
		}

		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		this->AttachToComponent(MainChar->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		MainChar->SetAttachedWeapon(this);
	}
}
