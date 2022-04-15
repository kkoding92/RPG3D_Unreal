// Copyright Epic Games, Inc. All Rights Reserved.


#include "RPG3DGameModeBase.h"
#include "ActionRPGCharacter.h"

ARPG3DGameModeBase::ARPG3DGameModeBase()
{
	DefaultPawnClass = AActionRPGCharacter::StaticClass();
}

