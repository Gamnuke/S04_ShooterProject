// Fill out your copyright notice in the Description page of Project Settings.

#include "LineProjectile.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

void ALineProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALineProjectile, TargetPoint);
	DOREPLIFETIME(ALineProjectile, ImpactNormal);
}

// Sets default values
ALineProjectile::ALineProjectile(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshScene = CreateDefaultSubobject<USceneComponent>(FName("MeshScene"), false);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"), false);
	MeshScene->SetupAttachment(GetRootComponent());
	Mesh->SetupAttachment(MeshScene);
}

// Called when the game starts or when spawned
void ALineProjectile::BeginPlay()
{
	Super::BeginPlay();
	TargetScale = FVector::Dist(GetActorLocation(), TargetPoint) / 100;
	StartTime = GetWorld()->TimeSeconds;
	EndTime = StartTime + Duration;
	StartLocation = GetActorLocation();
}

// Called every frame
void ALineProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetScale(TargetPoint, ImpactNormal);
	
}




void ALineProjectile::ServerSetVariables_Implementation(FVector newTargetPoint) { 
	SetVariables(newTargetPoint);
}
bool ALineProjectile::ServerSetVariables_Validate(FVector newTargetPoint) { return true; }
void ALineProjectile::SetVariables(FVector newTargetPoint) {
	TargetPoint = newTargetPoint;
	if (Role < ROLE_Authority) {
		ServerSetVariables(newTargetPoint);
	}
}







void ALineProjectile::SetScale(FVector newTargetPoint, FVector newImpactNormal) {
	float Alpha = FMath::Clamp(UKismetMathLibrary::NormalizeToRange(GetWorld()->TimeSeconds, StartTime, EndTime), 0.0f, 1.0f);
	float QuadAlpha = -4 * FMath::Pow(Alpha, 2) + 4 * Alpha;

	MeshScene->SetRelativeScale3D(FVector(TargetScale * QuadAlpha, GetActorScale3D().Y, GetActorScale3D().Z));
	if (Alpha >= 0.5) {
		SetActorLocation(newTargetPoint);
		SetActorRotation((StartLocation - newTargetPoint).Rotation());
	}
	if (Alpha >= 1) {

		Destroy();
	}

	if (Role < ROLE_Authority) {
		ServerSetScale(newTargetPoint, newImpactNormal);
	}
}
void ALineProjectile::ServerSetScale_Implementation(FVector newTargetPoint, FVector newImpactNormal) {
	SetScale(newTargetPoint, newImpactNormal);
}
bool ALineProjectile::ServerSetScale_Validate(FVector newTargetPoint, FVector newImpactNormal) { return true; }


