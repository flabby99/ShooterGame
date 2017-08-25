// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "../ActorPool.h"
#include "AI/Navigation/NavigationSystem.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavMeshOffset = FVector(0);
	MinExtent = FVector(0, -1900, 0);
	MaxExtent = FVector(4000, 1900, 0);
}

void ATile::SetPool(UActorPool* InPool)
{
	NavMeshPool = InPool;

	PositionNavMesh();
}

void ATile::PositionNavMesh()
{
	NavMesh = NavMeshPool->Checkout();
	if (!NavMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough Actors in pool"));
		return;

	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] Checked out"), *(NavMesh->GetName()));
	NavMesh->SetActorLocation(GetActorLocation() + NavMeshOffset);
	GetWorld()->GetNavigationSystem()->Build();
}

template<typename T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; ++i)
	{
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);
		bool bFound = FindEmptyLocation(SpawnPosition.Location, Radius * SpawnPosition.Scale);
		if (bFound)
		{
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPosition);
		}
	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius)
{
	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, 1.f, 1.f);
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FBox Bounds(MinExtent, MaxExtent);
	const int32 MAX_ATTEMPTS = 100;

	for (size_t i = 0; i < MAX_ATTEMPTS; ++i)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition &SpawnPosition)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	if (!Spawned) return;
	Spawned->SetActorRelativeLocation(SpawnPosition.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRelativeRotation(FRotator(0.f, SpawnPosition.Rotation, 0.f));
	Spawned->SetActorScale3D(FVector(SpawnPosition.Scale));
}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition &SpawnPosition)
{
	FRotator Rotation = FRotator(0.f , SpawnPosition.Rotation, 0.f );
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn, SpawnPosition.Location, Rotation);
	if (!Spawned) return;
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SpawnDefaultController();
	Spawned->Tags.Add(FName("Enemy"));
}
// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!NavMeshPool || !NavMesh) return;
	NavMeshPool->Return(NavMesh);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);

	bool bHasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);
	//FColor ResultColour = bHasHit ? FColor::Red : FColor::Green;
	//DrawDebugCapsule(GetWorld(), GlobalLocation, 10.f, Radius, FQuat::Identity, ResultColour, true);
	return !bHasHit;
}

