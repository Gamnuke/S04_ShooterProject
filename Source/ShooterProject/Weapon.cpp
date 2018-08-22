// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Projectile.h"
#include "UnrealNetwork.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"), false);
	WeaponMesh->SetupAttachment(GetRootComponent());
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Firing && GetWorld()->GetTimeSeconds() > NextShotTime) {
		FireWeaponNetworkHandler();
	}

	if (WeaponMesh != nullptr) {
		FrontGripSocket = WeaponMesh->GetSocketTransform(FName("FrontGrip"));
		BarrelSocket = WeaponMesh->GetSocketTransform(FName("Barrel"));
	}
}

void AWeapon::FireWeaponNetworkHandler() {
	//if (HasAuthority()) {
	if (Role == ROLE_AutonomousProxy || Role == ROLE_SimulatedProxy) {
		FireWeapon();
	}
	
	//}
	/*else {
		ServerFireWeapon();
	}*/
}

void AWeapon::FireWeapon() {
	NextShotTime = GetWorld()->GetTimeSeconds() + TimeBetweenShots;
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

	AProjectile *NewProj = GetWorld()->SpawnActor<AProjectile>(Projectile);
	NewProj->SetActorLocation(BarrelSocket.GetLocation());
	NewProj->ProjectileMesh->SetPhysicsLinearVelocity(FVector::UpVector * 2000);
}
void AWeapon::ServerFireWeapon_Implementation() {
	//FireWeapon();
}
bool AWeapon::ServerFireWeapon_Validate() { return true; }
void AWeapon::MulticastFireWeapon_Implementation() {
	FireWeapon();
}
void AWeapon::ClientFireWeapon_Implementation() {
	FireWeapon();
}


void AWeapon::SetFiring(bool bNewFiring) {
	Firing = bNewFiring;
}

