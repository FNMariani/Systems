// by FNM.


#include "SystemsGameState.h"

ASystemsGameState::ASystemsGameState()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> BP_ItemDB(TEXT("DataTable'/Game/Systems/Inventory/Data/ItemDB.ItemDB'"));
	
	ItemDB = BP_ItemDB.Object;
}

UDataTable* ASystemsGameState::GetItemDB() const
{
	return ItemDB;
}