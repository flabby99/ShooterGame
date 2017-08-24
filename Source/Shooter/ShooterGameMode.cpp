// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ShooterGameMode.h"
#include "ShooterHUD.h"
#include "Player/FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AI/Navigation/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "ActorPool.h"

AShooterGameMode::AShooterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Player/Behaviour/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AShooterHUD::StaticClass();

	NavMeshPool = CreateDefaultSubobject<UActorPool>(TEXT("Nav Mesh Pool"));
}

void AShooterGameMode::PopulateBoundsVolumePool()
{
	TActorIterator<ANavMeshBoundsVolume> VolumeIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	//Could do Template VarName(args)
	while (VolumeIterator)
	{
		NavMeshPool->Add(*VolumeIterator); //Get contents of the iterator.
		++VolumeIterator;
	}
}

UActorPool * AShooterGameMode::GetPool()
{
	return NavMeshPool;
}
