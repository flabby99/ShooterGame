// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "BallProjectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UGun::UGun()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(this);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}


// Called when the game starts
void UGun::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (MeshAsset) FP_Gun->SetSkeletalMesh(MeshAsset);
	FP_MuzzleLocation->AttachToComponent(FP_Gun, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Muzzle"));
	FP_MuzzleLocation->SetRelativeRotation(ProjectileRotation);
}


// Called every frame
void UGun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGun::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Instigator = Cast<APawn>(GetOwner());
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ABallProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetComponentLocation());
	}

	// try and play a firing animation if specified
	if (FPFireAnimation != NULL && TPFireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		if (FPAnimInstance != NULL && TPAnimInstance != NULL)
		{
			FPAnimInstance->Montage_Play(FPFireAnimation, 1.f);
			TPAnimInstance->Montage_Play(TPFireAnimation, 1.f);
		}
	}
}