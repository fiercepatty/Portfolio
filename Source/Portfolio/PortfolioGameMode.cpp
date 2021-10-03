// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortfolioGameMode.h"
#include "PortfolioPlayerController.h"
#include "PortfolioCharacter.h"
#include "UObject/ConstructorHelpers.h"

APortfolioGameMode::APortfolioGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APortfolioPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}