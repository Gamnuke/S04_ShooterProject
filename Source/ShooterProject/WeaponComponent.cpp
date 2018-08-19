// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "Projectile.h"
#include "Character1_CPP.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWeaponComponent::FireWeapon(FVector newAimLocation) {
	if (GetWorld()->GetTimeSeconds() > NextShotTime) {
		if (CharacterRef != nullptr) {
			CharacterRef->SetWeaponRotation();
		}
		NextShotTime = GetWorld()->GetTimeSeconds() + TimeBetweenShots;
		DrawDebugLine(
			GetWorld(),
			BarrelSocket.GetLocation(),
			newAimLocation,
			FColor::Orange,
			false,
			0.05,
			0,
			6
		);

		if (Projectile != nullptr) {
			AProjectile *NewProj = GetWorld()->SpawnActor<AProjectile>(Projectile);
			if (NewProj != nullptr) {
				NewProj->SetActorLocation(BarrelSocket.GetLocation());
				FRotator randRecoil = FRotator(FMath::RandRange(-5, 5), FMath::RandRange(-5, 5), FMath::RandRange(-5, 5));
				FVector Direction = ((newAimLocation - BarrelSocket.GetLocation()).Rotation() + randRecoil).Vector();
				NewProj->ProjectileMesh->SetPhysicsLinearVelocity(Direction * 6000);
				NewProj->ProjectileMesh->SetPhysicsAngularVelocity(Direction * 1000);
			}
		}
	}
}
void UWeaponComponent::ServerFireWeapon_Implementation(FVector newAimLocation) {
	FireWeapon(newAimLocation);
}
bool UWeaponComponent::ServerFireWeapon_Validate(FVector newAimLocation) { return true; }
 
