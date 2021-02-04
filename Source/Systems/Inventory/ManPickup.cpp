// by FNM.


#include "ManPickup.h"
#include "SystemsController.h"

AManPickup::AManPickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	RootComponent = Cast<USceneComponent>(PickupMesh);

	ItemID = FName("No ID");

	Super::Name = "Item";
	Super::Action = "pickup";
}

void AManPickup::Interact_Implementation(APlayerController* Controller)
{
	Super::Interact_Implementation(Controller);

	ASystemsController* IController = Cast<ASystemsController>(Controller);
	if (IController->AddItemToInventoryByID(ItemID))
		Destroy();
}