// by FNM.

#pragma once

#include "Engine.h"
#include "GameFramework/GameStateBase.h"
#include "SystemsGameState.generated.h"

/**
 * 
 */
UCLASS()
class SYSTEMS_API ASystemsGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ASystemsGameState();

	UDataTable* GetItemDB() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	class UDataTable* ItemDB;
};
