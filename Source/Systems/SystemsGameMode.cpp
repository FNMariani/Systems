// Copyright Epic Games, Inc. All Rights Reserved.

#include "SystemsGameMode.h"
#include "SystemsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASystemsGameMode::ASystemsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
