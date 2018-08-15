// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "Projectile.h"
#include "DrawDebugHelpers.h"


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
	if (WeaponMesh == nullptr) { return; }
	//FireWeaponNetworkHandler();
	ServerFireWeapon();

	if (WeaponMesh != nullptr) {
		FrontGripSocket = WeaponMesh->GetSocketTransform(FName("FrontGrip"));
		BarrelSocket = WeaponMesh->GetSocketTransform(FName("Barrel"));
	}
	// ...
}

void UWeaponComponent::FireWeaponNetworkHandler() {
	//if (HasAuthority()) {
	if (GetOwner()->Role == ROLE_AutonomousProxy || GetOwner()->Role == ROLE_SimulatedProxy) {
		ServerFireWeapon();
	}

	//}
	/*else {
	ServerFireWeapon();
	}*/
}

void UWeaponComponent::FireWeapon() {
	if (Firing && GetWorld()->GetTimeSeconds() > NextShotTime) {
		NextShotTime = GetWorld()->GetTimeSeconds() + TimeBetweenShots;
		UE_LOG(LogTemp, Warning, TEXT("SHOT WEAPON!"));
		DrawDebugLine(
			GetWorld(),
			BarrelSocket.GetLocation(),
			BarrelSocket.GetLocation() + BarrelSocket.GetRotation().GetForwardVector() * 10000,
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
				NewProj->ProjectileMesh->SetPhysicsLinearVelocity(BarrelSocket.GetRotation().GetForwardVector() * 2000);
			}
		}
	}
}
void UWeaponComponent::ServerFireWeapon_Implementation() {
	FireWeapon();
}
bool UWeaponComponent::ServerFireWeapon_Validate() { return true; }
void UWeaponComponent::MulticastFireWeapon_Implementation() {
	FireWeapon();
}
void UWeaponComponent::ClientFireWeapon_Implementation() {
	FireWeapon();
}


void UWeaponComponent::SetFiring(bool bNewFiring) {
	Firing = bNewFiring;
}

