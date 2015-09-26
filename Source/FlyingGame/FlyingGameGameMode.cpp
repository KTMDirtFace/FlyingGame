// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FlyingGame.h"
#include "FlyingGameGameMode.h"
#include "FlyingGamePawn.h"

AFlyingGameGameMode::AFlyingGameGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AFlyingGamePawn::StaticClass();
}
