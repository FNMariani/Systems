// by FNM.


#include "MoneyAutoPickup.h"
#include "SystemsController.h"

AMoneyAutoPickup::AMoneyAutoPickup()
{
	Super::ItemID = FName("money");
	Value = 1;
}

void AMoneyAutoPickup::Collect_Implementation(APlayerController* Controller)
{
	ASystemsController* IController = Cast<ASystemsController>(Controller);
	IController->Money += Value;
	Destroy();
}