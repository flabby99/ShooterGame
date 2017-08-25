// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

/**
 * Struct for a function
 */
USTRUCT()
struct FSpawnPosition
{
	GENERATED_USTRUCT_BODY()

	FVector Location;
	float Rotation;
	float Scale;
};


class UActorPool;

UCLASS()
class SHOOTER_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, Category = Spawn)
	void PlaceActors(TSubclassOf<AActor> ToSpawn, int32 MinSpawn = 1, int32 MaxSpawn = 1, float Radius = 500.f, float MinScale = 1.f, float MaxScale = 1.f);

	UFUNCTION(BlueprintCallable, Category = Spawn)
	void PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int32 MinSpawn = 1, int32 MaxSpawn = 1, float Radius = 200.f);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	* The amount to offset the navmesh on attach.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning")
	FVector NavMeshOffset;

	/**
	 * The min extent of bounding box
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning")
	FVector MinExtent;

	/**
	* The max extent of bounding box
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning")
	FVector MaxExtent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * This sets the Nav Mesh Pool reference from the game mode
	 * @param InPool - The pool reference to set
	 */
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPool(UActorPool* InPool);

private:
	bool CanSpawnAtLocation(FVector Location, float Radius);

	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	template<typename T>
	void RandomlyPlaceActors(TSubclassOf<T> ToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale);

	void PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition &SpawnPosition);

	void PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition &SpawnPosition);

	void PositionNavMesh();

	AActor* NavMesh;

	UActorPool* NavMeshPool;

};
