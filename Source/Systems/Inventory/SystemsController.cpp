// Fill out your copyright notice in the Description page of Project Settings.

#include "SystemsController.h"
#include "SystemsGameState.h"
#include "../SystemsCharacter.h"

ASystemsController::ASystemsController()
{
	InventorySlotLimit = 50;
	InventoryWeightLimit = 500;
}

int32 ASystemsController::GetInventoryWeight()
{
	int32 Weight = 0;
	for (auto& Item : Inventory)
	{
		Weight += Item.Weight;
	}

	return Weight;
}

bool ASystemsController::AddItemToInventoryByID(FName ID)
{
	ASystemsGameState* GameState = Cast<ASystemsGameState>(GetWorld()->GetGameState());
	UDataTable* ItemTable = GameState->GetItemDB();
	FInventoryItem* ItemToAdd = ItemTable->FindRow<FInventoryItem>(ID, "");

	if (ItemToAdd)
	{
		// If a Slot- or WeightLimit are not needed remove them in this line
		if (Inventory.Num() < InventorySlotLimit && GetInventoryWeight() + ItemToAdd->Weight <= InventoryWeightLimit)
		{
			Inventory.Add(*ItemToAdd);
			ReloadInventory();
			return true;
		}
	}
	return false;
}