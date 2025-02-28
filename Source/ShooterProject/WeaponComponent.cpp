// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "Projectile.h"
#include "Character1_CPP.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LineProjectile.h"

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

	if (!GetOwner()->HasAuthority()) {
		FVector InterpedLocation = FMath::VInterpTo(WeaponMesh->RelativeLocation, FVector::ZeroVector, DeltaTime, 20);
		WeaponMesh->SetRelativeLocation(InterpedLocation);
	}
	
}

void UWeaponComponent::FireWeapon(FVector newAimLocation) {
	if (CharacterRef == nullptr) { return; }
	if (GetWorld()->GetTimeSeconds() > NextShotTime) {
		CharacterRef->SetWeaponRotation();
		NextShotTime = GetWorld()->GetTimeSeconds() + TimeBetweenShots;
		ClientRecoilWeapon();

		// Applies spread to weapon fire
		FRotator randRecoil = FRotator(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f)) * Spread;
		FVector Direction = ((newAimLocation - BarrelSocket.GetLocation()).Rotation() + randRecoil).Vector().GetSafeNormal();

		// Line trace from barrel to the new spreadded location
		FHitResult SpreaddedTraj;
		GetWorld()->LineTraceSingleByChannel(
			SpreaddedTraj,
			BarrelSocket.GetLocation(),
			BarrelSocket.GetLocation() + Direction * 10000,
			CharacterRef->WeaponFireCC
		);
		FVector HitLocation;
		if (SpreaddedTraj.bBlockingHit) {
			HitLocation = SpreaddedTraj.ImpactPoint;
		}
		else {
			HitLocation = SpreaddedTraj.TraceEnd;
		}
		if (SpreaddedTraj.GetActor() != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("%s"), *SpreaddedTraj.GetActor()->GetName());
		}
		// Spawn projectile towards line trace hit location
		if (Projectile != nullptr) {
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ALineProjectile *NewProj = GetWorld()->SpawnActor<ALineProjectile>(Projectile, Params);

			if (NewProj != nullptr) {
				NewProj->SetActorLocation(BarrelSocket.GetLocation());
				NewProj->SetVariables(HitLocation);
				NewProj->SetActorRotation((HitLocation - BarrelSocket.GetLocation()).Rotation());
				NewProj->ImpactNormal = SpreaddedTraj.ImpactNormal;
				if (SpreaddedTraj.bBlockingHit) {
					NewProj->BounceOff = true;
				}
				NewProj->ProjectileBlueprint = Projectile;
			}
		}
		// Camera Recoil Effect
		ClientFireWeapon(FVector::ZeroVector);

	}

}
void UWeaponComponent::ServerFireWeapon_Implementation(FVector newAimLocation) {
	FireWeapon(newAimLocation);
}
bool UWeaponComponent::ServerFireWeapon_Validate(FVector newAimLocation) { return true; }
void UWeaponComponent::ClientFireWeapon_Implementation(FVector newAimLocation) {
	CharacterRef->CameraComp->SetFieldOfView(CharacterRef->CameraComp->FieldOfView - FOVIncrement);
}

void UWeaponComponent::ClientRecoilWeapon_Implementation() {
	if (WeaponMesh != nullptr) {
		WeaponMesh->AddRelativeLocation(FVector::ForwardVector * RecoilDistance);

		// SetWorldLocation(WeaponMesh->GetComponentLocation() - (WeaponMesh->GetForwardVector() * RecoilDistance));/
	}
}

void UWeaponComponent::MulticastFireWeapon_Implementation(FVector newAimLocation) {
	FireWeapon(newAimLocation);
}
 
